#include<stdio.h>
#include<conf.h>
#include<kernel.h>
#include<proc.h>

extern void printsyscallsummary();
extern int syscall_traceflag=0;

void syscallsummary_start()
{
	syscall_traceflag=1;
}

void syscallsummary_stop()
{
	syscall_traceflag=0;
}


void printsyscallsummary()
{
	
	struct pentry *process;
	int i, j;
	unsigned long avg;
	kprintf("\n\nvoid printsyscallsummary()");

        for(i = 0; i < NPROC; i++) 
	{
		int flag=0;
                process = &proctab[i];
 
                for(j = 0; j < 27; j++)
		{
                        if(process->syscall_count[j]==0)
				continue;
			if(flag==0)
			{
				kprintf("\nProcess [pid:%d]  %s",i,process->pname);
				flag=1;
			}
			avg=process->syscall_time[j]/process->syscall_count[j];
			kprintf("\n\tSyscall: %s, count: %d, average execution time: %ld", process->syscall_name[j],process->syscall_count[j], avg);
               	}
        }
return;
}
