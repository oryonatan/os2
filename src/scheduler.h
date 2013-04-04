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
#define NOT_SIGALARM  SIGVTALRM + 1
using namespace std;

enum state {
	READY, RUNNING, SUSPENDED, SLEEPING, TERMINATED, SIZE, NONE_SPECIFIED
};

typedef char stackMem[STACK_SIZE];
typedef void (*thread_functor)(void);

struct Thread {
	Thread(thread_functor func, int threadID);

	stackMem stack;
	state threadState;
	unsigned int id;
	thread_functor action;
	sigjmp_buf env;
	long sleepQuantoms;

	~Thread() {
		delete(stack);
		cout << "Thread deleted " << id << endl;
	}
};


//A struct for holding all the threads
struct ThreadsStruct {
	ThreadsStruct() :
			readyQueue(), suspended(), sleeping() {
		//TODO: create the main thread with ID=0
	}
	shared_ptr<Thread> running;
	list<shared_ptr<Thread>> readyQueue;
	set<shared_ptr<Thread>> suspended;
	set<shared_ptr<Thread>> sleeping;
	~ThreadsStruct(){
		readyQueue.clear();
		suspended.clear();
		sleeping.clear();
	}
};


class Scheduler {
public:
	//TODO - do we need a destructor (erase used_threads etc.)

	ThreadsStruct threads;
	//sets the scheduler at the library initialization
	int setup (int  quantumLength);
	void setQuantumLength (int quantum_usecs);
	shared_ptr<Thread> getThread (int tid);
	int terminateThread(shared_ptr<Thread>& targetThread);
	int suspendThread (shared_ptr<Thread>& targetThread);
	int sleepThread (int quantumNum);
	int quantumUpdate(int sig);
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
private:
	//we use this to fetch the threads by their id
	long quanta;
	int quantom_usecs;
	sigset_t mask;
	struct itimerval tv;
	struct sigaction action;

	//resets the timer in case the threads were switched before a quantum expired
	int resetTimer();
	void setRunningThread(shared_ptr<Thread>);

	//Cleans empty pointers to moved/deleted threads
	void eraseFromState (state originalState, shared_ptr<Thread> threadToErase);
};


//First initialize a scheduler , we need time handler to be static!
static Scheduler * schd = new Scheduler();
//calls sched quatomupdate (sched.quantomUpdate is not static);
static int timeHandler(int);

#endif



