#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>


int ldelete(int lockdescriptor) 
{	
	
	struct lock_entry *lptr = &lock_tab[lockdescriptor]; 
	int pid;
	lptr->lstate = DELETED;
	lptr->ltype = READ;
	lptr->lprio = 0;
	proctab[currpid].bitmask[lockdescriptor] = 0;
	
	int i=0;	
	while(i < NPROC) 
	{
		if(lptr->bitmask[i] == 1) 
		{
			struct pentry *pptr = &proctab[i];
			int j=0;
			int maxPrio = 0;
			while(j < NLOCKS) 
			{
				if(pptr->bitmask[j] == 1 && j != lockdescriptor) 
				{
					int max = findMax(j);
					if(maxPrio < max) 
					{
						maxPrio = max;
					}
				}
				j++;
			}
			pptr->pinh = maxPrio;
		}
		lptr->bitmask[i] = 0;
		
		i++;
	}
	
	lptr->nreaders = 0;
	
        if (nonempty(lptr->lqhead)) 
        {
        	pid=getfirst(lptr->lqhead);
                while( pid != EMPTY) 
                {
                    proctab[pid].plock_waitret = DELETED;
                    ready(pid,RESCHNO);
                }
                resched();
        }
        return(OK);
}

int findMax(int j) 
{
	struct lock_entry *lptr = &lock_tab[j];
	int i = q[lptr->lqhead].qnext;
	
	int max = 0;
	while(i != lptr->lqtail) 
	{
		if(max < getprio(i)) 
		{
			max = getprio(i);
		}
		
		i = q[i].qnext;
	}
	return max;
}
