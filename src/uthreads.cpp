#include "uthreads.h"
#include "scheduler.h"

Scheduler * sched = new Scheduler();

//performed in the case of system error;
//does the clenup and frees the memory
int shutDown ()
{
	delete sched;
	exit(1);
}

int uthread_init(int quantum_usecs) {

	sched->setQuantumLength(quantum_usecs);
	//creating the main thread
	sched->usedThreads[MAIN_THREAD_ID] = shared_ptr<Thread> (NULL, MAIN_THREAD_ID);
	sched->threads.running = sched->usedThreads[MAIN_THREAD_ID];
	//TODO - what happens to the stack when we start the main thread running
	if (sched->setMask() || sched->startTimer(quantum_usecs))
	{
		shutDown();
	}

	return OK;
}

int uthread_spawn(void (*f)(void)) {
//TODO - not sure we should put it here
	sched -> blockSignals;

	int id = sched->allocateID;
	if (id == FAIL)
		{
			cerr << "thread library error: maximum thread number exceeded" << endl;
			return FAIL;
		}

	sched->usedThreads[id] = shared_ptr<Thread> (new Thread(f, id));
	sched->threads.readyQueue.push_back(sched->usedThreads[id]);

	sched ->unblockSignals();
	return OK;
}

int uthread_terminate(int tid) {
	sched->blockSignals();

	shared_ptr<Thread> th= sched->getThread(tid);
	if ( th == NULL)
	{
		cerr << "thread library error: thread not found" <<endl;
		sched -> unblockSignals();
		return FAIL;
	}

	if (tid == MAIN_THREAD_ID)
	{
		//TODO - anything else needed in this case?
		delete sched;
		exit(0);
	}

	if (sched->terminateThread(th))
	{
		//TODO - check if a function could not terminate because of
		//thread library error
		shutDown();
	}

	sched->unblockSignals();
	return 0;
}

int uthread_suspend(int tid) {
	//TODO implement suspend by TID
	return 0;
}

int uthread_resume(int tid) {
	//TODO implement resume by TID
	return 0;
}

int uthread_sleep(int num_quantums) {

	return 0;
}

int uthread_get_tid() {
	return 0;
}

int uthread_get_total_quantums() {
	return 0;
}

int uthread_get_quantums(int tid) {
	return 0;
}
