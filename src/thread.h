/*
 * thread.h
 *
 *  Created on: Apr 6, 2013
 *      Author: yonatan
 */

#ifndef THREAD_H_
#define THREAD_H_



#include "uthreads.h"
#include <memory>
using namespace std;

enum state {
	READY, RUNNING, SUSPENDED, SLEEPING, TERMINATED, SIZE, NONE_SPECIFIED
};



typedef char stackMem[STACK_SIZE];
typedef void (*thread_functor)(void);




struct Thread {
	Thread(thread_functor func, int threadID);
	int totalQuanta;
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


#endif /* THREAD_H_ */
