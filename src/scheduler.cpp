#include "scheduler.h"
////////////////////////////////////////////////////////////////////////
/*				OS STUFF FROM DEMO_JUMP.cpp							////
 * /////////////////////////////////////////////////////////////////////
we should probably split to Thread.h/Thread.cpp
but meanwhile it's here
*/
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

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
static address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif

////////////////////////////////////////////////////////////////////////
//		ENDOF OS STUFF FROM DEMO_JUMP.cpp							////
////////////////////////////////////////////////////////////////////////


//For debug purposes: the number of allowed shared pointers owning the thread
#define COPIES_ALLOWED 3

void Scheduler::blockSignals() {
}

void Scheduler::unblockSignals() {
}

int Scheduler::allocateID() {
	for (int id = 1; id < MAX_THREAD_NUM; id++) {
		if (usedThreads.find(id) == usedThreads.end())
		{
			return id;
		}
//		try {
//			shared_ptr<Thread> temp = usedThreads.at(i);
//		}
//
//		catch (out_of_range&) {
//			return i;
//		}
	}

	return FAIL;

}

int Scheduler::quantumUpdate(int sig) {
	this->quanta++;
	for (shared_ptr<Thread> thread : threads.sleeping) {
		thread->sleepQuantoms--;
		if (thread->sleepQuantoms <= 0) {
			moveThread(thread, READY);
		}

	}
	if (!threads.readyQueue.empty()) {
		moveThread(move(threads.readyQueue.front()), RUNNING);
	}
	if (sig != SIGVTALRM)
	{
		resetTimer();
	}
}

shared_ptr<Thread> Scheduler::getThread(int tid) {

	shared_ptr<Thread> result = NULL;
	try {
		result = usedThreads.at(tid);
	} catch (out_of_range&) {
		return NULL;
	}

	return result;
}


static int timeHandler(int signum) {
	return schd->quantumUpdate(signum);
};

void Scheduler::eraseFromState(state originalState,
		shared_ptr<Thread> threadToErase) {
	switch (originalState) {
	case RUNNING:
		threads.running.reset();
		break;
	case READY:
		threads.readyQueue.remove(threadToErase);
		break;
	case SUSPENDED:
		threads.suspended.erase(threadToErase);
		break;
	case SLEEPING:
		threads.sleeping.erase(threadToErase);
		break;
	case NONE_SPECIFIED:
		break;

	}
}

void Scheduler::setRunningThread(shared_ptr<Thread> th) {
	cout << "new running thread " << th->id << ":" << th->env << endl;
	if (threads.running) {
		moveThread(move(threads.running), READY);
	}
	threads.running = move(th);
}

void Scheduler::moveThread(shared_ptr<Thread> th, state newState) {

	state originalState = th->threadState;
	eraseFromState(originalState, th);

	th->threadState = newState;
	switch (newState) {
	case RUNNING:
		setRunningThread(move(th));
		break;
	case READY:
		threads.readyQueue.push_back(move(th));
		break;
	case SUSPENDED:
		threads.suspended.insert(move(th));
		break;
	case SLEEPING:
		threads.sleeping.insert(move(th));
		break;
	case TERMINATED:
		//TODO?terminateThread(move(th));
		break;
	default:
		break;
	}

//	assert(th.use_count <= COPIES_ALLOWED);
}

int Scheduler::spawnThread(thread_functor func) {
	int id = allocateID();
	if (id == FAIL) {
		return FAIL;
	}

	usedThreads[id] = shared_ptr<Thread>(new Thread(func, id));
	threads.readyQueue.push_back(usedThreads[id]);

	return OK;

}

int Scheduler::terminateThread(shared_ptr<Thread>& th) {
	if (th->threadState == RUNNING) {
//		quantumUpdate (NOT_SIGALRM);
	}

	//TODO - remove the thread id from the control structures
}

void Scheduler::setQuantumLength(int quantum_usecs) {
	this->quantom_usecs = quantum_usecs;
}

int Scheduler::setMask() {
	if (sigemptyset(&mask)) {
		cerr << "system error: failed to initialize a mask" << endl;
		return SYSTEM_ERROR;
	}

	if (sigaddset(&mask, SIGVTALRM)) {
		cerr << "system error: could not add signal to mask" << endl;
		return SYSTEM_ERROR;
	}

	return OK;

}

int Scheduler::startTimer(int quantum_usec) {
	action.sa_handler =  (__sighandler_t)timeHandler;

	if (sigemptyset(&action.sa_mask)) {
		cerr << "system error: could not set an empty signal mask"	<< endl;
		return SYSTEM_ERROR;
	}

	if (sigaddset(&action.sa_mask, SIGVTALRM)) {
		cerr << "system error: could not add signal to mask" << endl;
		return SYSTEM_ERROR;
	}

	if (sigaction(SIGVTALRM, &action, NULL)) {
		cerr << "system error: could not create sigaction" <<endl;
		return SYSTEM_ERROR;
	}

	tv.it_value.tv_sec = quantum_usec/USECS_IN_SEC;
	tv.it_value.tv_usec = quantum_usec%USECS_IN_SEC;
	tv.it_interval.tv_sec = 0;
	tv.it_interval.tv_usec = 0;
	if(setitimer(ITIMER_VIRTUAL, &tv, NULL))
	{
		cerr<< "system error: failed to set timer"<<endl;
		return SYSTEM_ERROR;
	}

	return OK;

}

//resets the timer if threads were switched before the quantum
//expired. Assumes the timer was set before;
int Scheduler::resetTimer ()
{
	if(setitimer(ITIMER_VIRTUAL, &tv, NULL))
	{
		cerr<< "system error: failed to set timer"<<endl;
		return SYSTEM_ERROR;
	}

	return OK;

}


Thread::Thread(thread_functor func, int threadID):
		stack(),
		threadState(READY),
		id(threadID),
		action(func),
		sleepQuantoms(0) {
	//TODO if the number of threads exceeds the limit, delete the stack
	// and throw an exception
	//the id is done inside the scheduler - is it a good idea?
	address_t sp, pc;
	//why 1?
	sp = (address_t) this->stack + STACK_SIZE - sizeof(address_t);
	pc = (address_t) func;
	sigsetjmp(this->env,1);
	(this->env->__jmpbuf)[JB_SP] = translate_address(sp);
	(this->env->__jmpbuf)[JB_PC] = translate_address(pc);
	sigemptyset(&this->env->__saved_mask);
	cout << "Thread created : "<< id << endl;
}

