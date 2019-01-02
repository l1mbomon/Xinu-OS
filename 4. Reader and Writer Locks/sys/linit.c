#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

struct lock_entry lock_tab[NLOCKS];

void linit() 
{
	int i=0;
	struct lock_entry *lptr;
	while(i < NLOCKS) 
	{
		lptr = &lock_tab[i];
		
		lptr->lstate = LOCK_FREE;
		lptr->ltype = READ;
		lptr->lprio = 0;
		
		lptr->lqhead = newqueue();
		lptr->lqtail = lptr->lqhead + 1;
		
		
		int j=0;
		while(j < NPROC) 
		{
			lptr->bitmask[j] = 0;
			j++;
		}
		i++;
	}
}
