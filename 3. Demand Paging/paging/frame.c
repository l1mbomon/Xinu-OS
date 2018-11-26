/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */

SYSCALL init_frm()
{
	STATWORD ps; 
  	disable(ps);

	int i=0;
	fr_map_t frm_tab[NFRAMES];
  
	while(i < NFRAMES) 
	{
		frm_tab[i].fr_status = FRM_UNMAPPED;
		frm_tab[i].fr_pid = -1;
		frm_tab[i].fr_vpno = 4096;
		frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = FR_PAGE;
		frm_tab[i].fr_dirty = 0;
		frm_tab[i].fr_pte = 0;
	
		i++;
	}
	restore(ps);	
  	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
	STATWORD ps;
	disable(ps);
	
	int i;	
	for(i = 0; i < NFRAMES; i++) 
	{
        	if(frm_tab[i].fr_status == FRM_UNMAPPED) 
        	{
			*avail = i;
			restore(ps);
			return OK;
		}	
  	}

	switch(page_replace_policy)
	{
  		case SC:
  			{
  				int curr_pos = 0;
  				i= curr_pos;
  				
  				while(i < NFRAMES) 
  				{
					if(frm_tab[i].fr_type == FR_PAGE) 
					{
						pt_t* ptable = *frm_tab[i].fr_pte;
						if(ptable->pt_acc == 1)
						{
							ptable->pt_acc = 0;
							if(i == NFRAMES - 1) 
								i = curr_pos;
						}
						else 
						{
							curr_pos = i + 1;
							if(curr_pos == NFRAMES) 
							curr_pos = 0;
							break;
						}
					
					}
					
					i++;
				}	
  			
  			*avail = i;
			free_frm(*avail);
			restore(ps);
			return OK;
			}
  		case AGING:
  		{
  			i=0;
			while(i < NFRAMES) 
			{
				if(frm_tab[i].fr_type == FR_PAGE) 
				{
					int pid = frm_tab[i].fr_pid;
					int virtaddress= frm_tab[i].fr_vpno * NBPG;
					virt_addr_t *virt_addr = &virtaddress;
					
					pd_t *pd_entry = (proctab[pid].pdbr * NBPG) + (virt_addr->pd_offset * sizeof(pd_t));
					pt_t *pt_entry = (pd_entry->pd_base * NBPG) + (virt_addr->pt_offset * sizeof(pt_t));
					frm_tab[i].fr_refcnt = frm_tab[i].fr_refcnt>>1;
					if(pt_entry->pt_acc == 1) 
					{
						pt_entry->pt_acc = 0;
						frm_tab[i].fr_refcnt += 128;
						if(frm_tab[i].fr_refcnt > 255) 
						{
							frm_tab[i].fr_refcnt = 255;
						}					
						write_cr3(read_cr3());
					}
				}
				
				i++;
			}
			
			int min_age = 256;
			i=0;
			while(i < NFRAMES) 
			{
				if(frm_tab[i].fr_type == FR_PAGE && frm_tab[i].fr_refcnt < min_age)
                                        min_age = frm_tab[i].fr_refcnt;        
                                i++;
			}
		
  		
  			*avail = i;
	  		free_frm(*avail);
			restore(ps);
			return OK;
		}
  		default:
  			restore(ps);
  			return SYSERR;
  	}
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	STATWORD ps;
	disable(ps);

	int pid = frm_tab[i].fr_pid;
	int virtaddress = frm_tab[i].fr_vpno * NBPG;
	virt_addr_t *virt_addr;
	virt_addr = (virt_addr_t*)&virtaddress;
	
	
	pd_t *pd_entry = (proctab[pid].pdbr * NBPG) + (virt_addr->pd_offset * sizeof(pd_t));
	pt_t *pt_entry = (pd_entry->pd_base * NBPG) + (virt_addr->pt_offset * sizeof(pt_t));
	pt_entry->pt_pres = 0;
	pt_entry->pt_acc = 0;
	
	if(pid == currpid) 
		asm volatile("invlpg (%0)" ::"r" (pt_entry) : "memory");
		
		
	int fno = pd_entry->pd_base - FRAME0;
	frm_tab[fno].fr_refcnt--;
	
	if(frm_tab[fno].fr_refcnt == 0 && (page_replace_policy == SC || page_replace_policy == AGING)) 
	{
		frm_tab[fno].fr_status = FRM_UNMAPPED;
		frm_tab[fno].fr_pid = -1;
		frm_tab[fno].fr_vpno = 4096;
		frm_tab[fno].fr_type = FR_PAGE;
		pd_entry->pd_pres = 0;
	}

	int bs_no, pg_no, res;

	if(pt_entry->pt_dirty = 1)
	{
		res = bsm_lookup(pid, virtaddress, &bs_no, &pg_no);

		if( res == SYSERR)
			kill(pid);
			
		write_bs(((FRAME0 + i) * NBPG), bs_no, pg_no);
	}	
	
	restore(ps);
	return OK;
}
