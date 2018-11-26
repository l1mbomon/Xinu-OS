#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

bs_map_t bsm_tab[8];

SYSCALL release_bs(bsd_t bs_id) {
	STATWORD ps;
	disable(ps);
	
  /* release the backing store with ID bs_id */
  
	bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
	bsm_tab[bs_id].bs_pid = -1;
	bsm_tab[bs_id].bs_vpno = 4096;
	bsm_tab[bs_id].bs_npages = 0;
	bsm_tab[bs_id].bs_sem = 0;
	bsm_tab[bs_id].private_heap = 0;
	
	restore(ps);
	return OK;
}

