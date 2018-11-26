/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */

SYSCALL init_bsm()
{
	STATWORD ps;
	disable(ps);
	int i = 0;
	while(i < 8) 
	{
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;
		bsm_tab[i].bs_vpno = 0;
		bsm_tab[i].bs_npages = 0;
		bsm_tab[i].bs_sem = 0;
		bsm_tab[i].private_heap = 0;
		
		i++;
	}
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	STATWORD ps;
	disable(ps);
	int i = 0;
	
	while(i < 8) 
	{	if(bsm_tab[i].bs_status = BSM_UNMAPPED) 
		{
			*avail = i;
			restore(ps);
			return OK;
		}
		
		i++;
	}	
	
	restore(ps);
	return OK;	
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	STATWORD ps;
	disable(ps);
	
	bsm_tab[i].bs_status = BSM_UNMAPPED;
        bsm_tab[i].bs_pid = -1;
        bsm_tab[i].bs_vpno = 0;
       	bsm_tab[i].bs_npages = 0;
        bsm_tab[i].bs_sem = 0;
        bsm_tab[i].private_heap = 0;
	
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	STATWORD ps;
	disable(ps);
	
	int i=0;
	while(i < 8) 
	{
		if(bsm_tab[i].bs_pid == pid) 
		{
			*store = i;
			*pageth = (vaddr/NBPG) - bsm_tab[i].bs_vpno;
			restore(ps);
			return OK;
		}
		
		i++;
	}
	restore(ps);
	return SYSERR;			
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mappfing into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	STATWORD ps;
	disable(ps);
	if(source < 0 || source >= 8 || npages<=0 || npages > 256) {
		return SYSERR;
	}
	
	bsm_tab[source].bs_status = BSM_MAPPED;
	bsm_tab[source].bs_pid = pid;
	bsm_tab[source].bs_vpno = vpno;
	bsm_tab[source].bs_npages = npages;
	bsm_tab[source].bs_sem = 1;
	bsm_tab[source].private_heap = 0;
	
	restore(ps);
	return OK;		
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	STATWORD ps;
	disable(ps);
	
	int bs_no, pg_no;
        
        if(bsm_lookup(pid, vpno * NBPG, &bs_no, &pg_no) == SYSERR) 
        {
                restore(ps);
                return SYSERR;
        }       
        
        bsm_tab[bs_no].bs_status = BSM_UNMAPPED;
	bsm_tab[bs_no].bs_pid = -1;
        bsm_tab[bs_no].bs_vpno = 4096;
        bsm_tab[bs_no].bs_npages = 0;
        bsm_tab[bs_no].bs_sem = 0;
        bsm_tab[bs_no].private_heap = 0;
        
        restore(ps);
        return OK;
        
}


