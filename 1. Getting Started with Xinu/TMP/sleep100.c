/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL sleep100(int n)
{
        unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[20]="sys_sleep100";
                process->syscall_count[20]++;
        }

	
	STATWORD ps;    

	if (n < 0  || clkruns==0){
                unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[20]+=syscall_timerstart-syscall_timerstop;

	         return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep100(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*10);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[20]+=syscall_timerstart-syscall_timerstop;
	
	return(OK);
}
