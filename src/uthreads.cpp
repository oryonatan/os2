#include "uthreads.h"
#include "scheduler.h"
extern Scheduler * schd;

//performed in the case of system error;
//does the cleanup and frees the memory
int shutDown ()
{
	exit(1);
}

int uthread_init(int quantum_usecs) {
	schd->setQuantumLength(quantum_usecs);
	//creating the main thread
	schd->usedThreads[(MAIN_THREAD_ID)] =
			shared_ptr<Thread> (new Thread(NULL, MAIN_THREAD_ID));
	schd->threads.running = schd->usedThreads[MAIN_THREAD_ID];
	//TODO - what happens to the stack when we start the main thread running
	if (schd->setMask() || schd->startTimer(quantum_usecs))
	{
		exit(1);
	}
	schd->startTimer(quantum_usecs);
	return OK;

}

int uthread_spawn(void (*f)(void)) {
//TODO - not sure we should put it here
	schd -> blockSignals();

	int id = schd->allocateID();
	if (id == FAIL)
		{
			cerr << "thread library error: maximum thread number exceeded" << endl;
			return FAIL;
		}

	schd->usedThreads[id] = shared_ptr<Thread> (new Thread(f, id));
	schd->threads.readyQueue.push_back(schd->usedThreads[id]);

	schd ->unblockSignals();
	cerr << schd->threads.readyQueue.size() << endl;
	return OK;
}

int uthread_terminate(int tid) {
	schd->blockSignals();

	shared_ptr<Thread> th= schd->getThread(tid);
	if ( th == NULL)
	{
		cerr << "thread library error: thread not found" <<endl;
		schd -> unblockSignals();
		return FAIL;
	}

	if (tid == MAIN_THREAD_ID)
	{
		//TODO - anything else needed in this case?
		delete schd;
		exit(0);
	}

	if (schd->terminateThread(th))
	{
		//TODO - check if a function could not terminate because of
		//thread library error
		shutDown();
	}

	schd->unblockSignals();
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
