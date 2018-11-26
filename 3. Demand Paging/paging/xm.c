/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
bs_map_t bsm_tab[8];

SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
	STATWORD ps;
	disable(ps);
	
	if(bsm_tab[source].private_heap == 1 || bsm_map(currpid, virtpage, source, npages) == SYSERR) 
	{
		restore(ps);
		return SYSERR;
	}
	
	restore(ps);
	return OK;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
	STATWORD ps;
	disable(ps);
  
	if(virtpage < 4096) //virtual page lower bound
		return SYSERR;
		
  	bsm_unmap(currpid, virtpage, 0);
  	
  	restore(ps);
	return OK;
}
