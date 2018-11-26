/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */
extern unsigned long ctr1000;

SYSCALL	receive()
{
        unsigned long syscall_timerstart = ctr1000;
        struct pentry *process=&proctab[currpid];
        if(syscall_traceflag)
        {       process =&proctab[currpid];
                process->syscall_name[6]="sys_receive";
                process->syscall_count[6]++;
        }

	STATWORD ps;    
	struct	pentry	*pptr;
	WORD	msg;

	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);
        unsigned long syscall_timerstop = ctr1000;
        process->syscall_time[6]+=syscall_timerstart-syscall_timerstop;
	return(msg);
}
