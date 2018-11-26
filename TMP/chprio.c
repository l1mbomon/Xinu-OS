/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL chprio(int pid, int newprio)
{
	
	unsigned long syscall_timerstart = ctr1000;
	struct pentry *process=&proctab[currpid];
	if(syscall_traceflag)
	{	process =&proctab[currpid];
		process->syscall_name[1]="sys_chprio";
		process->syscall_count[1]++;
	}

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		
		unsigned long syscall_timerstop = ctr1000;
		process->syscall_time[1]+=syscall_timerstart-syscall_timerstop;
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	
	unsigned long syscall_timerstop = ctr1000;
	process->syscall_time[1]+=syscall_timerstart-syscall_timerstop;
	return(newprio);
}
