/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL resume(int pid)
{
	unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[9]="sys_resume";
                process->syscall_count[9]++;
        }

	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);

                unsigned long syscall_timerstop = ctr1000;
                process->syscall_time[9]+=syscall_timerstart-syscall_timerstop;		
		return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);

        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[9]+=syscall_timerstart-syscall_timerstop;
	return(prio);
}
