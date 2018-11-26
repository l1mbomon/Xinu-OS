#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sched.h>

scheduler_class = 0;	

void setschedclass(int sched_class){
	scheduler_class = sched_class;
}

int getschedclass(){
	return(scheduler_class);
}

int nextproc_exp(int randprio)
{
	int process, prev_proc;

	process = q[rdytail].qprev;
	prev_proc = q[process].qprev;

	if((randprio < q[prev_proc].qkey) && (prev_proc < NPROC))
	{
		if(q[process].qkey != q[prev_proc].qkey)
		{
			process = prev_proc;
		}
		prev_proc = q[prev_proc].qprev;
	}

	
	if (process >= NPROC)
		return(NULLPROC);
	else 
		return(process);

}


int getlast_exp(int proc_exp)
{
	
	if (proc_exp < NPROC)
		return(dequeue(proc_exp));
	else
		return(EMPTY);	
}
