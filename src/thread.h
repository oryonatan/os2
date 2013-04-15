/*
 * thread.h
 *
 *  Created on: Apr 6, 2013
 *      Author: yonatan,maria
 */

#ifndef THREAD_H_
#define THREAD_H_
#define THREAD_CREATION_FAILED "Failed to create thread: "


#include "uthreads.h"
#include <memory>
#include <iostream>
#include <list>
#include <set>
#include "setjmp.h"

using namespace std;

//states of a thread
enum state {
	READY, RUNNING, SUSPENDED, SLEEPING, TERMINATED, NONE_SPECIFIED
};


typedef char stackMem[STACK_SIZE];
typedef void (*thread_functor)(void);

//////////////////////////////////////////////////////////////////////////////
// The Thread struct is used to signify a specific thread					//
//////////////////////////////////////////////////////////////////////////////

struct Thread {
	//Total number of quantums a thread has run
	int totalQuanta;
	// The threads stack
	char* stack;
	// The current state of the thread
	state threadState;
	// The thread's id
	unsigned int id;
	// The thread's entry point
	thread_functor action;
	// Used to return correctly to the thread with siglongjump
	sigjmp_buf env;
	//if a thread sleeps - how long it needs to sleep
	long sleepQuantoms;

	Thread(thread_functor func, int threadID);
	~Thread() {
		delete(stack);
		cout << "Thread deleted " << id << endl;
	}
};


//A struct for holding all the threads
struct ThreadsStruct {
	ThreadsStruct() :
			readyQueue(), suspended(), sleeping() {
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
