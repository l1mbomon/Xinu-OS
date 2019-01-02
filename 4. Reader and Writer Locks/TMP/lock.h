#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef NLOCKS
#define NLOCKS 50
#endif

#define READ -10
#define WRITE -20

#define LOCK_FREE 1
#define LOCK_HELD 2

struct lock_entry 
{
	int lstate; 
	int ltype;
	int lprio;
	int nreaders;
	int nwriter;
	int lqhead;
	int lqtail;
	int bitmask[NLOCKS];
};

extern struct lock_entry lock_tab[NLOCKS]; 
extern int nextlock;

extern void linit();
extern int lcreate();
extern int lock();
extern int releaseall(int, int, ...);

#endif
