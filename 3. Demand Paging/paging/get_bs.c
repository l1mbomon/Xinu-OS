#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
  
	if(npages <= 0 || npages > 256 || bs_id >= 8 || bs_id < 0 || bsm_tab[bs_id].private_heap == 1)
		return(SYSERR);
		
	if(bsm_tab[bs_id].bs_npages == 0) 
	{
		bsm_tab[bs_id].bs_npages = npages;
		return npages;
	}
        return bsm_tab[bs_id].bs_npages;
}


