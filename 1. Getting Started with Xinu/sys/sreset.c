/* sreset.c - sreset */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include<lab0.h>
/*------------------------------------------------------------------------
 *  sreset  --  reset the count and queue of a semaphore
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;
SYSCALL sreset(int sem, int count)
{
        unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[22]="sys_sreset";
                process->syscall_count[22]++;
        }

	STATWORD ps;    
	struct	sentry	*sptr;
	int	pid;
	int	slist;

	disable(ps);
	if (isbadsem(sem) || count<0 || semaph[sem].sstate==SFREE) {
		restore(ps);
                unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[22]+=syscall_timerstart-syscall_timerstop;

		return(SYSERR);
	}
	sptr = &semaph[sem];
	slist = sptr->sqhead;
	while ((pid=getfirst(slist)) != EMPTY)
		ready(pid,RESCHNO);
	sptr->semcnt = count;
	resched();
	restore(ps);
        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[22]+=syscall_timerstart-syscall_timerstop;

	return(OK);
}
