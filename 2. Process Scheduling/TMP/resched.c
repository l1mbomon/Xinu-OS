#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <sched.h>
#include <stdio.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);

/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */


int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */


	int next_process, random_exp, optr_prio;				

	if (LINUXSCHED == scheduler_class) 
	{
		next_process = 0;

		optr = &proctab[currpid];
		optr_prio = optr->goodness - optr->counter;
		optr->goodness = optr_prio + preempt;
		optr->counter = preempt;
		
		if (currpid == NULLPROC || optr->counter <=0)
		{
			optr->counter = 0;
			optr->goodness = 0;
		}

		int proc_num = q[rdytail].qprev;
		int max_good = 0;
		
		while(proc_num != rdyhead)
		{
			if(proctab[proc_num].goodness > max_good)
			{
				next_process = proc_num;
				max_good = proctab[proc_num].goodness;
			}
			proc_num = q[proc_num].qprev;
		}

		if (optr->counter == 0 && max_good == 0)
		{
			int temp_pid = 0;
			struct pentry *temp_proc;

			while(temp_pid<NPROC)
			{
				temp_proc=&proctab[temp_pid];
				if(temp_proc->pstate!=PRFREE)
				{
					temp_proc->counter = temp_proc->pprio + (int)(temp_proc->counter)/2;
				}
				temp_proc->goodness =  temp_proc->pprio + temp_proc->counter;
				temp_pid++;
			}
			preempt = optr->counter;
		
			if (currpid == NULLPROC)
		 		return OK;
			else 
			{
				next_process = NULLPROC;
				#ifdef	RTCLOCK
					preempt = QUANTUM;
				#endif
			}
		}
		else if ((optr->goodness > 0) && (optr->goodness >= max_good))
		{
			if (optr->pstate == PRCURR)
			{
				preempt = optr->counter;
				return(OK);
			}
		}
		
		if (optr->pstate == PRCURR) 
		{
			optr->pstate = PRREADY;
			insert(currpid, rdyhead, optr->pprio);
		}

		nptr = &proctab[next_process];
		nptr->pstate = PRCURR;
		dequeue(next_process);
		currpid = next_process;
		preempt = nptr->counter;
		
		ctxsw((int) &optr->pesp, (int) optr->pirmask, (int) &nptr->pesp, (int) nptr->pirmask);
		return(OK);

	}

	else if (EXPDISTSCHED == scheduler_class)
	{
		random_exp = (int) expdev(0.1);
		next_process = nextproc_exp(random_exp);
		
		optr = &proctab[currpid];
			
		if ((q[next_process].qkey > optr->pprio) && (optr->pprio > random_exp) && (optr->pstate == PRCURR))
		{
			#ifdef	RTCLOCK
				preempt = QUANTUM;
			#endif
			return(OK);
		}			
	}
	else 
	{
		/* no switch needed if current process priority higher than next*/
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
			(lastkey(rdytail) < optr->pprio)) {
			return(OK);
		}
	}

	/* force context switch */
	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */

	if (scheduler_class == EXPDISTSCHED) 
	{
		currpid = getlast_exp(next_process);
	} 
	else 
	{
		currpid = getlast(rdytail);
	}

	nptr = &proctab[currpid];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter 	*/
	#endif

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	/* The OLD process returns here when resumed. */
	return OK;
}
