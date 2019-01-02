#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>


unsigned int ctr1000;


int releaseall(int numlocks, int ldes1, ...) 
{
	if(lock_tab[ldes1].lstate == LOCK_FREE) 
		return SYSERR;
	
	int max_prio = 0;
 	unsigned long int *lock_addr = &ldes1;
	
	int i=0;
	while(i < numlocks) 
	{
		release_lock(*(lock_addr + i));
		i++;
	}	 
	
	int j=0;
	while(j < NLOCKS) 
	{
		if(max_prio < lock_tab[j].lprio) 
			if(proctab[currpid].bitmask[j] == 1)
				max_prio = lock_tab[j].lprio;
		j++;
	}
	proctab[currpid].pinh = max_prio; 
	return OK;
}

int release_lock(int l1) 
{
	struct pentry *pptr = &proctab[currpid];
	struct lock_entry *lptr = &lock_tab[l1];
	
	lptr->bitmask[currpid] = 0;
	pptr->bitmask[l1] = 0;
	
	if(lptr->ltype == READ) 
	{
		lptr->nreaders--;
		if(lptr->nreaders > 0) 
			return OK;
	}
	
	if(lptr->nreaders == 0 || lptr->ltype == WRITE) 
	{
		int nextProc = findNextProc(l1);
	
		if(nextProc == -1) 
		{
			lptr->lstate = LOCK_FREE; 
			return OK;
		}
		else 
		{
			if(proctab[nextProc].lock_type_req[l1] == READ) 
			{
				lock_tab[l1].nreaders++;
				
				int hpw_prio = findHighestPriorityWriter(l1);
				
				int j =	q[lptr->lqtail].qprev;		
				while(j != lptr->lqhead) 
				{
					if(q[j].qkey >= hpw_prio)
						if(proctab[j].lock_type_req[l1] == READ)
						{
							proctab[j].lockid = -1;
							proctab[j].bitmask[l1] = 1;
        	                			lptr->bitmask[j] = 1;
							
        	                			
        	                			dequeue(j);
        	                			ready(j,RESCHNO);
						}
					j = q[j].qprev;
				}
					 
				lock_tab[l1].ltype = READ;
			}
			else 
			{
				lock_tab[l1].ltype = WRITE;
			}
			
			proctab[nextProc].lockid = -1;
			proctab[nextProc].bitmask[l1] = 1;
			lptr->bitmask[nextProc] = 1;
			lptr->lprio = 0;
			
			dequeue(nextProc);
			ready(nextProc,RESCHNO);
		}
	}
}

int findHighestPriorityWriter(int l1) {

	int max_prio = 0;

	struct lock_entry *lptr = &lock_tab[l1];
	
	int i = q[lptr->lqhead].qnext;
	while(i != lptr->lqtail) 
	{
		if(proctab[i].lock_type_req[l1] == WRITE && q[i].qkey > max_prio) 
		{
			max_prio = q[i].qkey;
		}
		i = q[i].qnext;
	}
	return max_prio;
}


int findNextProc(int lock) 
{
	struct lock_entry *lptr = &lock_tab[lock];
	int temp_proc_stack[50];
	int nextProc = -1;
	int max_Prio = -1000;
	int flag = 0;
	int index = 0;
	
	int i = q[lptr->lqtail].qprev;
	
	while(i != lptr->lqhead) 
	{
		if(max_Prio < q[i].qkey) 
		{
			max_Prio = q[i].qkey;
			nextProc = i;
			flag = 0;
		}
		else if(max_Prio == q[i].qkey) 
		{
			flag = 1;
			temp_proc_stack[index++] = i;
		}
		else 
		{
			continue;
		}
		i = q[i].qprev;
	}
	if(flag == 0) 
		return nextProc;

	else if(flag == 1)
	{
		int wait_time = 0;
	
		int j = 0;
		while(j < index) 
		{
			int wait = ctr1000 - proctab[temp_proc_stack[j]].wait_time;
			
			if(wait > wait_time) 
			{
				wait_time = wait;
				nextProc = temp_proc_stack[j];
			}
			j++;
		}
		
		if(proctab[nextProc].lock_type_req[lock] == WRITE) 
			return nextProc;
		else 
		{
			
			int possible_next_proc = 0;
			int new_wait = 0;
			
			int k=0;
			while(k < index) 
			{
				if(proctab[temp_proc_stack[k]].lock_type_req[lock] == WRITE) 
				{
					int wait = ctr1000 - proctab[temp_proc_stack[k]].wait_time;
					
					if(wait > new_wait) 
					{
						possible_next_proc = temp_proc_stack[k];
						new_wait = wait;
					}	
				}
				k++;
			}
			if(possible_next_proc != 0)
				if(wait_time - new_wait < 1000)
					return possible_next_proc;
		}
		return nextProc;
	}
}

