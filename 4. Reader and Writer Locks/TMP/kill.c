/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	int 	i;

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
			i=0;
			while(i < NLOCKS) 
			{
				if(proctab[pid].bitmask[i] == 1)
					releaseall(1,i);
				i++;
			}
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;
			int lock = proctab[pid].lockid;
			struct lock_entry *lptr = &lock_tab[lock];
			int max_prio = -100;
			
			if(lptr->lprio == proctab[pid].pprio) 
			{
				int k = q[lptr->lqhead].qnext;
				while(k != lptr->lqtail) 
				{
					if(k != pid) 
						if(max_prio < getprio(k))
							max_prio = getprio(k);
					k = q[k].qnext;
				}
				
				lptr->lprio = max_prio;
				
				int j=0;
				while(j < NPROC) 
				{
					if(lptr->bitmask[j] == 1) 
					{
						if(max_prio > proctab[j].pprio) 
						{
							proctab[j].pinh = max_prio;
						}
						else 
						{
							proctab[j].pinh = 0;
						}
					}
					j++;
				}
			}
			
			i=0;
			while(i < NLOCKS) 
			{
				if(proctab[pid].bitmask[i] == 1) 
					releaseall(1,i);
				i++;
			}
			pptr->pstate = PRFREE;	

	case PRREADY:	dequeue(pid);
	
			i=0;
			while(i < NLOCKS) 
			{
				if(proctab[pid].bitmask[i] == 1) 
					releaseall(1,i);
				i++;
			}
			
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
			
			i=0;
			while(i < NLOCKS) 
			{
				if(proctab[pid].bitmask[i] == 1) 
					releaseall(1,i);
				i++;
			}
			
	}
	restore(ps);
	return(OK);
}
