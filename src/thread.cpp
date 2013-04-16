/*
 * thread.cpp
 *
 *  Created on: Apr 13, 2013
 *      Author: yonatan,maria
 */
#include "thread.h"
#include "setjmp.h"
#include <iostream>
#include "signal.h"
using namespace std;


#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
		"rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */

address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif


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
	//cerr << "Thread created : "<< id << endl; //Debug
}
