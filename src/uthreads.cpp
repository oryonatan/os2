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
	if (schd->setup(quantum_usecs) == FAIL) return FAIL;
	return OK;
}

int uthread_spawn(void (*f)(void)) {
	schd -> blockSignals();

	int id = schd->allocateID();
	if (id == FAIL)
		{
			cerr << "thread library error: maximum thread number exceeded" << endl;
			schd->unblockSignals();
			return FAIL;
		}

	schd->usedThreads[id] = shared_ptr<Thread> (new Thread(f, id));
	schd->threads.readyQueue.push_back(schd->usedThreads[id]);
	//cerr << schd->threads.readyQueue.size() << endl; //Debug
	schd->unblockSignals();
	return id;
}

int uthread_terminate(int tid) {
	schd->blockSignals();
	shared_ptr<Thread> th = schd->getThread(tid);
	if (th == NULL) {
		cerr << "thread library error: thread not found" << endl;
		schd->unblockSignals();
		return FAIL;
	}

	if (tid == MAIN_THREAD_ID) {
		delete schd;
		exit(0);
	}

	schd->terminateThread(th);
	schd->unblockSignals();
	return 0;
}

int uthread_suspend(int tid) {

	schd->blockSignals();
	if (tid == 0)
	{
		cerr << "thread library error: the main thread cannot be suspended";
		schd->unblockSignals();
		return FAIL;
	}

	shared_ptr<Thread> th  = schd->getThread(tid);
	if (th == NULL)
	{
		//the error message was printed with getThread
		schd->unblockSignals();
		return FAIL;
	}

	if (th->threadState == SUSPENDED || th->threadState == SLEEPING)
	{
		schd->unblockSignals();
		return OK;
	}
	else
	{
		schd->unblockSignals();
		int ret_val = sigsetjmp(schd->threads.running->env,1);
			  if (ret_val == 1) {
			      return OK;
			  }

			schd->suspendThread(th);
			siglongjmp(schd->threads.running->env,1);
			return OK;
	}

	schd->unblockSignals();
	return 0;
}

int uthread_resume(int tid) {

	shared_ptr<Thread> th = schd->getThread(tid);
	if (th == NULL)
	{
		//schd->unblockSignals();
		return FAIL;
	}

	schd->resumeThread(th);
	return OK;
}

int uthread_sleep(int num_quantums) {

	schd->blockSignals();
	int tid = uthread_get_tid();
	if ( 0 == tid){
		schd->unblockSignals();
		cerr << "thread library error: the main thread cannot be put to sleep" << endl;
		return FAIL;
	}
	int ret_val = sigsetjmp(schd->threads.running->env,1);
	  if (ret_val == 1) {
		  schd->unblockSignals();
	      return OK;
	  }
	  //We will later subtract 1 from the sleeping quantums of all sleeping threads, including the
	  //one that has been put to sleep. But it has just started sleeping - we're giving it and additional
	  //quantum
	schd->sleepRunning(num_quantums+1);
	siglongjmp(schd->threads.running->env,1);
	schd->unblockSignals();
	return OK;
}

int uthread_get_tid() {
	return schd->threads.running->id;
}

int uthread_get_total_quantums() {
	return schd->quanta;
}

int uthread_get_quantums(int tid) {
	shared_ptr<Thread> th = schd->getThread(tid);
	if (th == NULL)
	{
		return FAIL;
	}

	return th->totalQuanta;
}
