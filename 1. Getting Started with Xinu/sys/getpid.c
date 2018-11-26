/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL getpid()
{
	
	unsigned long syscall_timerstart = ctr1000;
	struct pentry *process=&proctab[currpid];
	if(syscall_traceflag)
	{	process =&proctab[currpid];
		process->syscall_name[2]="sys_getpid";
		process->syscall_count[2]++;
	}

	unsigned long syscall_timerstop = ctr1000;
	process->syscall_time[2]+=syscall_timerstart-syscall_timerstop;
	return(currpid);
}
