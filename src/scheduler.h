#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define OK 0
#define FAIL -1
#define STACK_SIZE 4096
#define MAX_THREAD_NUM 100
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
using namespace std;

enum state {
	READY, RUNNING, SUSPENDED, SLEEPING, TERMINATED, SIZE, NONE_SPECIFIED
};

typedef char stackMem[STACK_SIZE];
typedef void (*thread_functor)(void);

struct Thread {
	stackMem stack;
	state threadState;
	unsigned int id;
	thread_functor action;
	sigjmp_buf env;
	long sleepQuantoms;

	static unsigned int thread_id;

	Thread(thread_functor func, int threadID):
			stack(), threadState(READY), id(threadID), action(func) {
		//TODO if the number of threads exceeds the limit, delete the stack
		// and throw an exception
		//the id is done inside the scheduler - is it a good idea?

		cout << "Thread created : "<< id << endl;

	};


	~Thread() {
		delete(stack);
		cout << "Thread deleted " << id << endl;
	}
};

unsigned int Thread::thread_id = 0;

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

	ThreadsStruct threads;
	//sets the scheduler at the library initialization
	void setup (int  quantumLength);
	shared_ptr<Thread> getThread (int tid);
	int spawnThread(thread_functor func);
	int terminateThread(shared_ptr<Thread>& targetThread);
	int suspendThread (shared_ptr<Thread>& targetThread);
	int sleepThread (int quantumNum);
	void quantumUpdate(int sig);
	//Moves a thread to the appropriate list
	void moveThread(shared_ptr<Thread>, state);

	//blocking and unblocking signals during performing an operation
	void blockSignals();
	void unblockSignals();

private:
	//we use this to fetch the threads by their id
	unordered_map <int,shared_ptr<Thread> > usedThreads;
	long quanta;
	int quantom_usecs;
	//sets the mask for signal blocking
	void setMask();
	void startTimer();
	int allocateID();
	void setRunningThread(shared_ptr<Thread>);
	//Cleans empty pointers to moved/deleted threads
	void cleanEmptyThreads(state);
	void eraseFromState (state originalState, shared_ptr<Thread> threadToErase)
};

#endif



