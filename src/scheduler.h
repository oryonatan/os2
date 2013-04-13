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

#define MOVE_THREAD_FAIL "thread library error: Failed to move thread"m
#define SIGEMPTYSET_FAIL "system error: Failed to initiate empty signal mask"
#define SIGADDSET_FAIL "system error: Failed to add signal to mask"
#define SIGMASK_FAIL "system error: failed to create signal mask"
#define UNBLOCK_FAIL  "system error: failed to unblock signals"
#define MASK_SET_FAIL	"system error: failed to initialize a mask:"
#define MASK_ADD_FAIL 	"system error: could not add signal to mask:"
#define EMPTY_SET_FAIL 	"system error: could not set an empty signal mask"
#define SIGACTION_FAIL 	"system error: could not create sigaction"
using namespace std;


class Scheduler {
public:
	int setup(int quantomLength);
	void getDebugData ();
	//TODO - do we need a destructor (erase used_threads etc.)

	ThreadsStruct threads;
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
	int startTimer();
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

//Handler for timer events
static void timeHandler(int signum) {
	cout << "Quantum has passed" << endl;
	int ret_val = sigsetjmp(schd->threads.running->env,1);
	  if (ret_val == 1) {
	      return;
	  }
	schd->quantumUpdate(signum);
	schd->startTimer();
	siglongjmp(schd->threads.running->env,1);
};





#endif



