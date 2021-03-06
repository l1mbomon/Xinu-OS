/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <paging.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
int i;
fr_map_t frm_tab[NFRAMES];
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	for(i = 0; i < 1023; i++) 
	{
		if(frm_tab[i].fr_pid == pid)
		{
		 	if(frm_tab[i].fr_type == FR_PAGE) 
			{
				free_frm(i);
				frm_tab[i].fr_pid = -1;
                        	frm_tab[i].fr_type = FR_PAGE;
                        	frm_tab[i].fr_status = FRM_UNMAPPED;
                        	frm_tab[i].fr_vpno = 4096;
			}
		}
		if(frm_tab[i].fr_pid == pid)
		{ 
			if(frm_tab[i].fr_type == FR_DIR) 
			{
				frm_tab[i].fr_pid = -1;
                        	frm_tab[i].fr_type = FR_PAGE;
                        	frm_tab[i].fr_status = FRM_UNMAPPED;
                        	frm_tab[i].fr_vpno = 4096;
			}
		}
	}
	
	release_bs(proctab[pid].store);	
	restore(ps);
	return(OK);
}
