/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL signal(int sem)
{
	unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[16]="sys_signal";
                process->syscall_count[16]++;
        }
	
	STATWORD ps;    
	register struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);

                unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[16]+=syscall_timerstart-syscall_timerstop;
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);

        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[16]+=syscall_timerstart-syscall_timerstop;
	return(OK);
}
