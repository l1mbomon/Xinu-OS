/* freemem.c - freemem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mem.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  freemem  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */

extern unsigned long ctr1000;

SYSCALL	freemem(struct mblock *block, unsigned size)
{
	// for printsyscallsummary
	unsigned long syscall_timerstart = ctr1000;
	struct pentry *process=&proctab[currpid];
	if(syscall_traceflag)
	{	process =&proctab[currpid];
		process->syscall_name[0]="sys_freemem";
		process->syscall_count[0]++;
	}


	STATWORD ps;    
	struct	mblock	*p, *q;
	unsigned top;

	if (size==0 || (unsigned)block>(unsigned)maxaddr
	    || ((unsigned)block)<((unsigned) &end))
	{	

		unsigned long syscall_timerstop = ctr1000;
		process->syscall_time[0]+=syscall_timerstart-syscall_timerstop;
		return(SYSERR);
	}
	size = (unsigned)roundmb(size);
	disable(ps);
	for( p=memlist.mnext,q= &memlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);

		unsigned long syscall_timerstop = ctr1000;
		process->syscall_time[0]+=syscall_timerstart-syscall_timerstop;
	
		return(SYSERR);
	}
	if ( q!= &memlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore(ps);

	unsigned long syscall_timerstop = ctr1000;
	process->syscall_time[0]+=syscall_timerstart-syscall_timerstop;
	return(OK);
}
