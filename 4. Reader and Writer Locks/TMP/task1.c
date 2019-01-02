#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
//#include <lock.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }
            
int mystrncmp(char* des,char* target,int n)
{
    int i;
 
    for (i=0;i<n;i++)
    {
        if (target[i] == '.') 
        	continue;
        if (des[i] != target[i]) 
        	return 1;
    }
    return 0;
}


void reader (char *msg, int lck)
{
        int     ret;
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock\n", msg);
	sleep(3);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void writer (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, sleep 6s\n", msg);
        sleep (6);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}


void tester_proc1 (char *msg)
{
        kprintf("Test proc going to sleep 10s\n");
        sleep(10);
        return;
}

void test_for_lock ()
{
        int     lck;
        int     rd1, rd2;
        int     wr1;
	int     t1;
	int pr1;
        kprintf("\nTest : Test the Priority Inversion problem\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 3 failed");
        wr1 = create(writer, 2000, 22, "writer1", 2, "writer1", lck);
        rd1 = create(reader, 2000, 30, "reader1", 2, "reader1", lck);
        pr1 = create(tester_proc1, 2000, 25, "process 1", 2, "process 1");
        kprintf("-start writer1, then sleep 1s\n");
        resume(wr1);
        sleep (2);
        kprintf("-start reader1, reader 1 will be blocked then sleep 1s\n");
        resume(rd1);
        sleep(2);
	assert(getprio(wr1) == 30, "Test case failed");
        kprintf("-start proc 1.\n");
        resume(pr1);
	sleep(2);
}



void sema_reader (char *msg, int sem)
{
        int     ret;
        kprintf ("  %s: to acquire lock\n", msg);
        wait (sem);
        kprintf ("  %s: acquired lock\n", msg);
        kprintf ("  %s: to release lock\n", msg);
        signal(sem);
}

void sema_writer (char *msg, int sem)
{
        kprintf ("  %s: to acquire lock\n", msg);
       	wait(sem);
        kprintf ("  %s: acquired lock, sleep 10s\n", msg);
        sleep (10);
        kprintf ("  %s: to release lock\n", msg);
        signal(sem);
}

void tester_proc (char *msg)
{
	kprintf("Tester Proc runs, this is priority inversion problem\n");
	kprintf("Going to sleep 10s");
	sleep(10);
	return;
}


void test_for_semaphore() 
{
	kprintf("Test: Testing sempahores\n");
	int swr1, srd2, srd1, pr1;
	int sem = screate(1);
	swr1 = create(sema_writer, 2000, 15, "swriter1", 2, "swriter1", sem);
        srd1 = create(sema_reader, 2000, 30, "sreader1", 2, "sreader1", sem);
	pr1 = create(tester_proc, 2000, 25, "tester_proc", 1, "proc1");
        kprintf("-start writer1, then sleep 1s\n");
        resume(swr1);
        sleep (2);
	kprintf("-start reader1, reader 1 will be blocked then sleep 1s\n");
	resume(srd1);
	assert(getprio(swr1) == 30, "Test case 3 failed");
	sleep(2);
	kprintf("-start proc 1\n");
	resume(pr1);
	sleep(2);
}

int main() 
{
	test_for_semaphore();
	test_for_lock();
	shutdown();
	return 0;
}
