/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */

extern unsigned long ctr1000;

SYSCALL	setdev(int pid, int dev1, int dev2)
{
	unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[13]="sys_setdev";
                process->syscall_count[13]++;
        }

	short	*nxtdev;

	if (isbadpid(pid))
	{
                unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[13]+=syscall_timerstart-syscall_timerstop;
		return(SYSERR);
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[13]+=syscall_timerstart-syscall_timerstop;
	return(OK);
}
