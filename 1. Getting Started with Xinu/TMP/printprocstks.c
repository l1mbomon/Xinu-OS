#include <kernel.h>
#include <proc.h>
#include <stdio.h>

extern void printprocstks();

unsigned long *esp;

void printprocstks(int priority)
{

	int id=0;
	struct pentry *process=proctab;

	kprintf("\n\nvoid printprocstks(int priority)\n");

	while(id<NPROC)
	{	
		if(process->pprio>priority)
		{

			kprintf("\nProcess [%s]", process->pname);
			kprintf("\n\tpid: %d", id);
			kprintf("\n\tpriority: %d", process->pprio);
			kprintf("\n\tbase: 0x%08x", process->pbase);
			kprintf("\n\tlimit: 0x%08x", process->plimit);
		        kprintf("\n\tlen: %d", process->pstklen);

			if(id==currpid)
			{
				asm("movl %esp, esp");
				kprintf("\n\tpointer: 0x%08x", esp);
			}
			
			else
			{
				kprintf("\n\tpointer: 0x%08x", process->pesp);
			}

		}

		process++;
		id++;
	}
	
	return;
}
