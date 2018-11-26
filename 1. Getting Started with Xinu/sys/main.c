/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */

//extern int zfunction(long param);
int prx;
tester(c)
int c;
{
	//printf("%d", trace_flag);
	        c = getpid();
	//          	//sleep(2);
	//          	        //printsyscallsummary();
}

extern unsigned long ctr1000;
extern unsigned long clktime;

int main()
{
	syscallsummary_start();
	kprintf("\n\nHello World, Xinu lives\n\n");
	long res = zfunction(0xaabbccdd);
	printf("value: %x\n", res);

	printsegaddress();
	printtos();
	printprocstks(2);
	resume(prx=create(tester,2000,15,"tester process",1,1));
	int a = getpid();
	sleep(1);
	//printf("\n\n%d ------------------  %d\n\n",ctr1000,clktime);
	signal(1);
	syscallsummary_stop();
	printsyscallsummary();
	return 0;}
