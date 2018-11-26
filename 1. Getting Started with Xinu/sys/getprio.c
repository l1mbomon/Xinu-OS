/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL getprio(int pid)
{
	unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[3]="sys_getprio";
                process->syscall_count[3]++;
        }

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
                
		unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[3]+=syscall_timerstart-syscall_timerstop;
		return(SYSERR);
	}
	restore(ps);
	
	unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[3]+=syscall_timerstart-syscall_timerstop;
	return(pptr->pprio);
}
