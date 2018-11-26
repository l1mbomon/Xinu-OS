/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */

SYSCALL pfint()
{
	STATWORD ps;

	pd_t *pd_entry;
	pt_t *pt_entry;


	disable(ps);
	
	int virtaddress;
	virt_addr_t *virt_addr;
	virtaddress = read_cr2();
	virt_addr = (virt_addr_t*)&virtaddress;
	
	pd_entry = (proctab[currpid].pdbr * NBPG) + virt_addr->pd_offset * sizeof(pd_t);
	int fno;
	
	if(pd_entry->pd_pres !=1)
	{
		int i=0;
		get_frm(&fno);
                pt_entry = (FRAME0 + fno) * NBPG;
                
                while(i < 1024) 
                {
			pt_entry->pt_pres = 0;
			pt_entry->pt_write = 0;
			pt_entry->pt_user = 0;
			pt_entry->pt_pwt = 0;
			pt_entry->pt_pcd = 0;
			pt_entry->pt_acc = 0;
			pt_entry->pt_dirty = 0;
			pt_entry->pt_mbz = 0;
			pt_entry->pt_global = 0;
			pt_entry->pt_avail = 0;
			pt_entry->pt_base = 0;
			
			i++;
		}
			
		frm_tab[fno].fr_status = FRM_MAPPED;
		frm_tab[fno].fr_type = FR_TBL;
		frm_tab[fno].fr_pid = currpid;
		
		pd_entry->pd_pres = 1;
		pd_entry->pd_write = 1;
		pd_entry->pd_user = 0;
		pd_entry->pd_pwt = 0;
		pd_entry->pd_pcd = 0;
		pd_entry->pd_acc = 0;
		pd_entry->pd_mbz = 0;
		pd_entry->pd_fmb = 0;
		pd_entry->pd_global = 0;
		pd_entry->pd_avail = 0;
		pd_entry->pd_base = (FRAME0 + fno);

	}
	
	pt_entry = ((pd_entry->pd_base*NBPG)+virt_addr->pt_offset * sizeof(pt_t));
	int bs_no, pg_no;	
	int pt_fno;
	
	if(pt_entry->pt_pres != 1)
	{
                frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt++;
		get_frm(&pt_fno);
		
                frm_tab[pt_fno].fr_status = FRM_MAPPED; 
                frm_tab[pt_fno].fr_type = FR_PAGE;
                frm_tab[pt_fno].fr_pid = currpid;
                frm_tab[pt_fno].fr_vpno = virtaddress/NBPG;
                
		if(page_replace_policy == AGING)
			frm_tab[pt_fno].fr_refcnt = 0; 
		
		bsm_lookup(currpid, virtaddress, &bs_no, &pg_no);
                read_bs(((FRAME0+pt_fno)*NBPG), bs_no, pg_no);
                
		pt_entry->pt_pres = 1;
                pt_entry->pt_write = 1;
		pt_entry->pt_acc = 1;
		
                pt_entry->pt_base = FRAME0 + pt_fno;	
		frm_tab[pt_fno].fr_pte = &pt_entry;
	}
	write_cr3(proctab[currpid].pdbr * NBPG);
	restore(ps);
  	return OK;
}
