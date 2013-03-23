#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define OK 0
#define FAIL -1
#define STACK_SIZE 4096
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <unordered_map> >
using namespace std;

enum state {
	READY, RUNNING, SUSPENDED, SLEEPING, TERMINATED, SIZE, NONE_SPECIFIED
};

typedef char stackMem[STACK_SIZE];

struct Thread {
	stackMem stack;
	state threadState;
	unsigned int id;
	sigjmp_buf env;
	long sleepQuantoms;

	Thread() :
		//TODO ID should be incremental , should accept functor
			stack(), threadState(READY),id(rand()) {
		cout << "Thread created : "<< id << endl;

	};


	~Thread() {
		delete(stack);
		cout << "Thread deleted " << id << endl;
	}
};

//A struct for holding all the threads
//Usage of unique_ptr prevents a case that the same thread exists in two different states.
struct ThreadsStruct {
	ThreadsStruct() :
			readyQueue(), suspended(), sleeping() {
	}
	unique_ptr<Thread> running;
	list<unique_ptr<Thread>> readyQueue;
	set<unique_ptr<Thread>> suspended;
	set<unique_ptr<Thread>> sleeping;
	~ThreadsStruct(){
		readyQueue.clear();
		suspended.clear();
		sleeping.clear();
	}
};


class Scheduler {
public:

	ThreadsStruct threads;
	bool setTimeInterval(int);
	void terminateThread(unique_ptr<Thread> );
	void updateRunning();
	//Moves a thread to the appropriate list
	void moveThread(unique_ptr<Thread>, state);
	state hasThread(int);
private:
	//we can use this to tell if a thread exists
	unordered_map <int,state> usedThreads;
	long quanta;
	int quantom_usecs;
	void setRunningThread(unique_ptr<Thread>);
	//Cleans empty pointers to moved/deleted threads
	void cleanEmptyThreads(state);
};

#endif



