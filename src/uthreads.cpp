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
	//TODO if setMask is successful, we start the timer twice, is it OK?
	schd->startTimer(quantum_usecs);
	return OK;

}

int uthread_spawn(void (*f)(void)) {
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
	//TODO - remove after debugging
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

	if (tid == 0)
	{
		cerr << "thread library error: the main thread cannot be suspended";
		return FAIL;
	}

	shared_ptr<Thread> th  = schd->getThread(tid);
	if (th == NULL)
	{
		//the error message was printed with getThread
		return FAIL;
	}

	if (th->threadState == SUSPENDED || th->threadState == SLEEPING)
	{
		return OK;
	}
	else
	{
		schd->suspendThread(th);
	}

	return 0;
}

int uthread_resume(int tid) {
	shared_ptr<Thread> th = schd->getThread(tid);
	if (th == NULL)
	{
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
	schd->startTimer(schd->quantom_usecs);
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
	/*try	{
		return schd->usedThreads.at(tid)->totalQuanta;
	}catch (out_of_range&)
	{
		return FAIL;
	}*/
}
