#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <lock.h>
#include <stdio.h>

unsigned int ctr1000;

int lock(int ldes1, int type, int priority) 
{
	struct pentry *pptr;
	struct lock_entry *lptr = &lock_tab[ldes1];

	int i;
	
	if(proctab[currpid].plock_waitret == DELETED) 
		return SYSERR;

	if((proctab[currpid].bitmask[ldes1] == 1 && lock_tab[ldes1].bitmask[currpid] == 0) || lock_tab[ldes1].lstate == DELETED) 
	{
		return SYSERR;
	}

	if(lock_tab[ldes1].lstate == LOCK_FREE) 
	{
		lock_tab[ldes1].lstate = LOCK_HELD;
		lock_tab[ldes1].ltype = type;
		lock_tab[ldes1].bitmask[currpid] = 1;
		
		proctab[currpid].lock_type_req[ldes1] = type;
		proctab[currpid].bitmask[ldes1] = 1;
		if(type == READ) lock_tab[ldes1].nreaders++;
	}
	else if(lock_tab[ldes1].ltype == READ) 
	{
		if(type == READ) 
		{	
			int i = q[lock_tab[ldes1].lqhead].qnext;
			int hpw_exists=0;
			
			while(i != lock_tab[ldes1].lqtail) 
			{
				if(q[i].qkey > priority) 
				{
					if(proctab[i].lock_type_req[ldes1] == WRITE) 
					{
						hpw_exists = 1;
					}
				}
				i = q[i].qnext;
			}
			
			
			if(hpw_exists == 1) 
			{
				if(proctab[currpid].pprio > lock_tab[ldes1].lprio) 
				{
                                	lock_tab[ldes1].lprio = proctab[currpid].pprio;

                                	for(i = 0; i < NPROC; i++) 
                                	{
                                        	if(lock_tab[ldes1].bitmask[i] == 1)
                                        	{
                                        		if (proctab[i].pinh < proctab[currpid].pprio) 
                                        		{
                 		                               	proctab[i].pinh = proctab[currpid].pprio;
								check_transitivity(i);
							}
						}
                 	     		}
				}

				(pptr = &proctab[currpid])->pstate = PRWAIT;
                        	pptr->lockid = ldes1;
                        	
                        	pptr->lock_type_req[ldes1] = type;
				pptr->wait_time = ctr1000;
				insert(currpid, lptr->lqhead, priority);
				pptr->plock_waitret = OK;
                        	resched();
                        	return pptr->plock_waitret;
			}
			
 			lock_tab[ldes1].nreaders++;
			lock_tab[ldes1].bitmask[currpid] = 1;

			proctab[currpid].lock_type_req[ldes1] = type;
			proctab[currpid].bitmask[ldes1] = 1;
			return OK;
		}
		else 
		{
			if(proctab[currpid].pprio > lock_tab[ldes1].lprio) 
			{
				lock_tab[ldes1].lprio = proctab[currpid].pprio;	
				for(i = 0; i < NPROC; i++) 
				{
					if(lock_tab[ldes1].bitmask[i] == 1)
					{ 
						if(proctab[i].pinh < proctab[currpid].pprio) 
						{
							proctab[i].pinh = proctab[currpid].pprio;
							check_transitivity(i);
						}
					}
				}
			}
			
			(pptr = &proctab[currpid])->pstate = PRWAIT;
                	pptr->lockid = ldes1;
                	
			pptr->lock_type_req[ldes1] = type;
			pptr->wait_time = ctr1000;
                	insert(currpid, lptr->lqhead, priority);
                	pptr->plock_waitret = OK;
                	resched();
                	return pptr->plock_waitret;
		}
	}	
	else if(lock_tab[ldes1].ltype == WRITE) 
	{
		struct pentry *pptr = &proctab[currpid];
		pptr->pstate = PRWAIT;
		
		proctab[currpid].lock_type_req[ldes1] = type;

		if(proctab[currpid].pprio > lock_tab[ldes1].lprio) 
		{
                	lock_tab[ldes1].lprio = proctab[currpid].pprio;
                        for(i = 0; i < NPROC; i++) 
                        {
                        	if(lock_tab[ldes1].bitmask[i] == 1)
                        	{
                        		if(proctab[i].pinh < proctab[currpid].pprio) 
                        		{
                                		proctab[i].pinh = proctab[currpid].pprio;
						check_transitivity(i);
					}
				}
                        }
                }
		pptr->lockid = ldes1;
		pptr->wait_time = ctr1000;
		insert(currpid, lptr->lqhead, priority);
		pptr->plock_waitret = OK;
		resched();
		return pptr->plock_waitret;
	}
	return OK;
}

void check_transitivity(int i) 
{
	if(proctab[i].lockid == -1) 
		return;
	int k=0;
	int lock = proctab[i].lockid;
	while(k < NPROC) 
	{
		if(getprio(k) < proctab[i].pinh) 
		{
			if(lock_tab[lock].bitmask[k] == 1) 
			{
				proctab[k].pinh = proctab[i].pinh;
				check_transitivity(k);
			}
		}
		k++;
	}
	return;
}

