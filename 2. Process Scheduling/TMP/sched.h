#ifndef _sched_h_
#define _sched_h_

#define EXPDISTSCHED 1
#define LINUXSCHED 2


int getlast_exp(int process);
int nextproc_exp(int randprio);

int getschedclass();
void setschedclass(int sched_class);

extern int scheduler_class;
#endif

