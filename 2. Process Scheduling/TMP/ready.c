/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sched.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int ready(int pid, int resch)
{
	register struct	pentry	*pptr;

	if (isbadpid(pid))
		return(SYSERR);
	pptr = &proctab[pid];
	pptr->pstate = PRREADY;

	int new_prio;

	if(scheduler_class == LINUXSCHED)
	{
		new_prio = pptr->pprio + pptr->counter;
		insert(pid,rdyhead,new_prio);		
	}
	else
	{
		insert(pid,rdyhead,pptr->pprio);
	}
	
	if (resch)
	{
		//kprintf("\ncurrent proc name: %s", proctab[currpid].pname);
		resched();
	}
	return(OK);
}
