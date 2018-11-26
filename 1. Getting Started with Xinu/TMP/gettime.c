/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
#include <date.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL	gettime(long *timvar)
{
	unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[4]="sys_gettime";
                process->syscall_count[4]++;
        }

    /* long	now; */

	/* FIXME -- no getutim */
        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[4]+=syscall_timerstart-syscall_timerstop;
    	
	return OK;
}
