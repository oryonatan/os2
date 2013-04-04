#include "scheduler.h"
//For debug purposes: the number of allowed shared pointers owning the thread
#define COPIES_ALLOWED 3

void Scheduler::blockSignals() {
}

void Scheduler::unblockSignals() {
}

int Scheduler::allocateID() {
	for (int i = 1; i < MAX_THREAD_NUM; i++) {
		try {
			shared_ptr<Thread> temp = usedThreads.at(i);
		}

		catch (out_of_range&) {
			return i;
		}
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

