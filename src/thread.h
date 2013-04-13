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

enum state {
	READY, RUNNING, SUSPENDED, SLEEPING, TERMINATED, NONE_SPECIFIED
};


typedef char stackMem[STACK_SIZE];
typedef void (*thread_functor)(void);


struct Thread {
	int totalQuanta;
	stackMem stack;
	state threadState;
	unsigned int id;
	thread_functor action;
	sigjmp_buf env;
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
