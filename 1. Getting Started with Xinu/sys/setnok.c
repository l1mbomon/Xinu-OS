/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */

extern unsigned long ctr1000;

SYSCALL	setnok(int nok, int pid)
{
        unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[14]="sys_setnok";
                process->syscall_count[14]++;
        }


	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
                unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[14]+=syscall_timerstart-syscall_timerstop;

		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);
        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[14]+=syscall_timerstart-syscall_timerstop;

	return(OK);
}
