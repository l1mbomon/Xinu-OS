#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

LOCAL int newlock();

int lcreate() 
{
	
	int lock=newlock();
	
	if(lock == SYSERR) 
	{
		return SYSERR;
	}
	return lock;
}

LOCAL int newlock()
{
        int     currlock;
        int     i=0;

        while (i<NLOCKS) 
        {
                currlock=nextlock--;
                if (nextlock < 0)
                        nextlock = NLOCKS-1;
		
		if(lock_tab[currlock].lstate == LOCK_FREE) 
		{
			return currlock;
		}		 
        
        	i++;
        }
        return(SYSERR);
}
