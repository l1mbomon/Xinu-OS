/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL	wait(int sem)
{
        unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[26]="sys_wait";
                process->syscall_count[26]++;
        }


	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
                unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[26]+=syscall_timerstart-syscall_timerstop;

		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
                unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[26]+=syscall_timerstart-syscall_timerstop;

		return pptr->pwaitret;
	}
	restore(ps);
        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[26]+=syscall_timerstart-syscall_timerstop;

	return(OK);
}
