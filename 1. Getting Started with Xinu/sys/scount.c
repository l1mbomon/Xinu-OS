/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;
SYSCALL scount(int sem)
{
	unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[10]="sys_scount";
                process->syscall_count[10]++;
        }


extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE)
	{
		unsigned long syscall_timerstop = ctr1000;
        	process->syscall_time[10]+=syscall_timerstart-syscall_timerstop;
		return(SYSERR);
	}
	unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[10]+=syscall_timerstart-syscall_timerstop;
	return(semaph[sem].semcnt);
}
