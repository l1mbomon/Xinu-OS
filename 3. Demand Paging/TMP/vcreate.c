/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

bs_map_t bsm_tab[8];

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	int pid, bsm_no;; 
	pid = create(procaddr, ssize, priority, name, nargs, args);
	
	if(get_bsm(&bsm_no) == SYSERR) 
		return SYSERR;	
		
	bsm_map(pid,4096,bsm_no,hsize);
	
	proctab[pid].store = bsm_no;
	proctab[pid].vhpno = 4096;
	proctab[pid].vhpnpages = hsize;
	
	struct mblock *vheap = 4096 * NBPG;
	vheap->mlen = hsize * NBPG;
	vheap->mnext = NULL;
	proctab[pid].vmemlist->mnext = vheap;
	
	struct mblock *head = BACKING_STORE_BASE + (bsm_no * BACKING_STORE_UNIT_SIZE);
	
	head->mlen = hsize * NBPG;
	head->mnext = NULL;
	
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
