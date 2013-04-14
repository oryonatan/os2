/*
 * thread.cpp
 *
 *  Created on: Apr 13, 2013
 *      Author: yonatan,maria
 */
#include "thread.h"
#include "address_translation.h"
#include "setjmp.h"
#include <iostream>
#include "signal.h"
using namespace std;

//Ctor for thread object , exit(1) on fail
Thread::Thread(thread_functor func, int threadID):
		stack((char *) malloc (STACK_SIZE)),
		threadState(READY),
		id(threadID),
		action(func),
		sleepQuantoms(0),
		totalQuanta(0)
{
	//DEBUG
	if (NULL == func)
	{
		cerr << "Main created : "<< id << endl;
		return;
	}

	address_t sp, pc;
	sp = (address_t) this->stack + STACK_SIZE - sizeof(address_t);
	pc = (address_t) func;

	if (sigsetjmp(this->env,1)){
			cerr << THREAD_CREATION_FAILED <<"sigsetjmp" << endl;
			exit(1);
	}

	(this->env->__jmpbuf)[JB_SP] = translate_address(sp);
	(this->env->__jmpbuf)[JB_PC] = translate_address(pc);

	if (sigemptyset(&this->env->__saved_mask)){
		cerr << THREAD_CREATION_FAILED << "sigemptyset"<< endl;
		exit(1);
	}
	//DEBUG
	cerr << "Thread created : "<< id << endl;
}
