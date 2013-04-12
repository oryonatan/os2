#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define OK 0
#define FAIL -1
#define SYSTEM_ERROR -2
#define MAIN_THREAD_ID  0
#define STACK_SIZE 4096
#define MAX_THREAD_NUM 100
#define USECS_IN_SEC 1000000
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <setjmp.h>
#include <signal.h>
#include <utility>
#include <unistd.h>
#include <sys/time.h>
#include <stdexcept>
#include <unordered_map>
#include <assert.h>
#include "thread.h"
#define NOT_SIGALARM  SIGVTALRM + 1
using namespace std;


class Scheduler {
public:
	int getThreadsCount(){return usedThreads.size();};
	int getReadyCount(){return threads.readyQueue.size();};
	int readySet(){return threads.running != NULL;};
	void getDebugData ();
	//TODO - do we need a destructor (erase used_threads etc.)

	ThreadsStruct threads;
	//sets the scheduler at the library initialization
	//int setup (int  quantumLength);
	void setQuantumLength (int quantum_usecs);
	shared_ptr<Thread> getThread (int tid);
	int terminateThread(shared_ptr<Thread>& targetThread);
	void  suspendThread (shared_ptr<Thread>& targetThread);
	void resumeThread (shared_ptr <Thread>& targetThread);
	void sleepRunning (int quantumNum);
	void quantumUpdate(int sig);
	//Moves a thread to the appropriate list
	void moveThread(shared_ptr<Thread>, state);
	int spawnThread(thread_functor);

	//sets the mask for signal blocking
	int setMask();
	int startTimer(int quantum_usec);
	//blocking and unblocking signals during performing an operation
	void blockSignals();
	void unblockSignals();

	int allocateID();
	unordered_map <int,shared_ptr<Thread> > usedThreads;
	int quantom_usecs;
	long quanta;
private:

	sigset_t mask;
	struct itimerval tv;
	struct sigaction action;

	//resets the timer in case the threads were switched before a quantum expired
	int resetTimer();
	void setRunningThread(shared_ptr<Thread>);

	//Cleans empty pointers to moved/deleted threads
	void eraseFromState (state originalState, shared_ptr<Thread> threadToErase);
};



extern Scheduler * schd;
static void timeHandler(int signum) {

	cout << "Quantum has passed" << endl;
	//TODO I'm not quite sure this works , I want to set the jump
	//to the current thread
	int ret_val = sigsetjmp(schd->threads.running->env,1);
	  if (ret_val == 1) {
	      return;
	  }
	schd->quantumUpdate(signum);
	//the running thread should may be changed now , if it wasn't we will
	//jump to the same location (I think , this really hasn't been tested
	schd->startTimer(schd->quantom_usecs);
	siglongjmp(schd->threads.running->env,1);
};


#endif



