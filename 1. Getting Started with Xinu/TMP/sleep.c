/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL	sleep(int n)
{
        unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[18]="sys_sleep";
                process->syscall_count[18]++;
        }

	STATWORD ps;    
	if (n<0 || clkruns==0)
	{
	       	unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[18]+=syscall_timerstart-syscall_timerstop;
		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
                unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[18]+=syscall_timerstart-syscall_timerstop;
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);


        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[18]+=syscall_timerstart-syscall_timerstop;
	return(OK);
}
