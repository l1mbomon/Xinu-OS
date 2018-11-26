#include <stdio.h>

extern void printtos();

unsigned long *esp; 
unsigned long *ebp;


void printtos()
{	
	int count;

        asm("movl %esp,esp");
	asm("movl %ebp,ebp");

        kprintf("\n\nvoid printtos()\n");

	kprintf("\nBefore[0x%08x]: 0x%08x",ebp+2,*(ebp+2));
	kprintf("\nAfter[0x%08x]: 0x%08x",ebp,*(ebp));
	
	for(count=0; count<4; count++)
	{
                kprintf("\n\telement[0x%08x] is : 0x%08x",(esp+count),*(esp+count));  
        
	}       
        return;
}   

