#include "scheduler.h"

#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
using namespace std;

//DEBUG :For debug purposes: the number of allowed shared pointers owning the thread
#define COPIES_ALLOWED 3

//Block signals
void Scheduler::blockSignals() {
	{
		//cout << "blocking signals" << endl; //Debug
		sigset_t sigMask;
		if (sigemptyset(&sigMask)) {
			cerr << SIGEMPTYSET_FAIL << endl;
			exit(1);
		}
		if (sigaddset(&sigMask, SIGVTALRM)) {
			cerr << SIGADDSET_FAIL << endl;
			exit(1);
		}
		if (sigprocmask(SIG_SETMASK, &sigMask, &mask)) {
			cerr << SIGMASK_FAIL << endl;
			exit(1);

		}
		return;
	}
}

//Unblock signals
void Scheduler::unblockSignals() {
	//cout << "Unblocking signals" << endl; //Debug
	if (sigprocmask(SIG_SETMASK, &mask, NULL)) {
		cerr << UNBLOCK_FAIL << endl;
		exit(1);
	}
}

//Allocates an id to thread
int Scheduler::allocateID() {
	for (int id = 1; id < MAX_THREAD_NUM; id++) {
		if (usedThreads.find(id) == usedThreads.end()) {
			return id;
		}
	}
	return FAIL;

}

//Updates the system when a new quantum was allocated
void Scheduler::quantumUpdate(int sig) {
	//cout << "Quantum Passed" << endl; //Debug
	if (!threads.suspended.empty())
	{
		shared_ptr<Thread> temp = *(threads.suspended.begin());
		//cout << "Suspended thread: " << temp->id << endl; //Debug
	}

	schd->quanta++;
	for (shared_ptr<Thread> thread : threads.sleeping) {
		thread->sleepQuantoms--;
		if (thread->sleepQuantoms <= 0) {
			moveThread(thread, READY);
		}
	}
	if (!threads.readyQueue.empty()) {
		moveThread(threads.readyQueue.front(), RUNNING);
	}

	//this function is used both when the times expires and when we're sleeping/terminating//
	//suspending the thread before a quantum has expired. In the latter case, the timer must be reset
	if (sig != SIGVTALRM)
	{
		resetTimer();
	}


	//cout << threads.running->id << endl;//DEBUG
}

//Gets a thread
shared_ptr<Thread> Scheduler::getThread(int tid) {
	shared_ptr<Thread> result = NULL;
	try {
		result = usedThreads.at(tid);
	} catch (out_of_range&) {
		cerr << THREAD_NOT_FOUND << endl;
		return NULL;
	}
	return result;
}

//Suspends a thread
void Scheduler::suspendThread(shared_ptr<Thread>& targetThread) {
	//suspending a sleeping/suspended thread is not an error
	state originalState = targetThread->threadState;
	if (originalState  == SUSPENDED
			|| originalState == SLEEPING) {
		return;
	}

	else {
		moveThread(targetThread, SUSPENDED);
		//if a thread suspended itself, a scheduling decision needs to be made
		if (originalState == RUNNING) {
			quantumUpdate(NOT_SIGALARM);
		}
		return;
	}
}

//Resumes a sleeping thread
void Scheduler::resumeThread(shared_ptr<Thread>& targetThread) {
	state originalState = targetThread->threadState;
	if (originalState == SUSPENDED) {
		//cout << "Thread " << targetThread->id << endl; //Debug
		moveThread(targetThread, READY);
	}
	return;
}

//sets the running thread to sleep
void Scheduler::sleepRunning(int quantumNum) {
	shared_ptr<Thread> running =threads.running;
	if (running->id == 0) return; // cant sleep main
	running->sleepQuantoms = quantumNum;
	moveThread(running,SLEEPING);
	//Whenever a new thread starts running, we treat it as if a quantum has expired
	quantumUpdate(NOT_SIGALARM);
}

//Initial setup for the scheduler, set quantom length and main thread.
int Scheduler::setup(int quantomLength) {
	this->setQuantumLength(quantomLength);
	if(MAX_THREAD_NUM <1 ) return FAIL;
	try {
		this->usedThreads[(MAIN_THREAD_ID)] = shared_ptr<Thread>(
				new Thread(NULL, MAIN_THREAD_ID));
		this->threads.running = schd->usedThreads[MAIN_THREAD_ID];
		this->setMask();
		this->startTimer();
		return OK;
	} catch (...) {
		return FAIL;
	}
	return OK; // this actually does'nt do anything , but eclipse like this line
}

//Remove a thread from its queue
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
	case TERMINATED:
		break;
	case NONE_SPECIFIED:
		break;
	}
}

//Sets the thread to running state,noves the running to ready
void Scheduler::setRunningThread(shared_ptr<Thread> th) {
	//cerr << "new running thread " << th->id << " : " << th->env << endl; //Debug
	if (threads.running) {
		moveThread(threads.running, READY);
	}
	threads.running = th;
	th->totalQuanta++;

}

//Moves a thread to another list
void Scheduler::moveThread(shared_ptr<Thread> th, state newState) {
	state originalState = th->threadState;
	eraseFromState(originalState, th);

	th->threadState = newState;
	switch (newState) {
	case RUNNING:
		setRunningThread(th);
		break;
	case READY:
		threads.readyQueue.push_back(th);
		break;
	case SUSPENDED:
		threads.suspended.insert(th);
		break;
	case SLEEPING:
		threads.sleeping.insert(th);
		break;
	case TERMINATED:
		usedThreads.erase (th->id);
		break;
	default:
		break;
	}
}

//Terminates a thread
void  Scheduler::terminateThread(shared_ptr<Thread>& th) {

	state thState = th->threadState;
	moveThread(th, TERMINATED);

	if (thState == RUNNING) {
		quantumUpdate(NOT_SIGALARM);
		siglongjmp(schd->threads.running->env,1);
	}

}

//Sets the qunatom length
void Scheduler::setQuantumLength(int quantum_usecs) {
	Scheduler::quantom_usecs = quantum_usecs;
}

//Sets a signal mask
int Scheduler::setMask() {
	if (sigemptyset(&mask)) {
		cerr << MASK_SET_FAIL << "sigemptyset" << endl;
		exit(1);
	}

	if (sigaddset(&mask, SIGVTALRM)) {
		cerr << MASK_ADD_FAIL << "sigaddset" << endl;
		exit(1);
	}

	return OK;

}

//Starts the times
int Scheduler::startTimer() {
	action.sa_handler =  (__sighandler_t)timeHandler;

	if (sigemptyset(&action.sa_mask)) {
		cerr << EMPTY_SET_FAIL << endl;
		exit(1);
	}

	if (sigaddset(&action.sa_mask, SIGVTALRM)) {
		cerr << MASK_ADD_FAIL << "sigaddset" << endl;
		exit(1);
	}

	if (sigaction(SIGVTALRM, &action, NULL)) {
		cerr << SIGACTION_FAIL << endl;
		exit(1);
	}

	tv.it_value.tv_sec = quantom_usecs/USECS_IN_SEC;
	tv.it_value.tv_usec = quantom_usecs%USECS_IN_SEC;
	tv.it_interval.tv_sec = 0;
	tv.it_interval.tv_usec = 0;
	if(setitimer(ITIMER_VIRTUAL, &tv, NULL))
	{
		cerr<< SETITIMER_FAIL<<endl;
		exit (1);
	}

	return OK;

}


int isPending (int sig)
{
	sigset_t signalSet;

	if (sigpending (&signalSet) != 0)
	{
		cerr << SIGPENDING_FAIL << endl;
		exit (1);
	}

	int caught = sigismember (&signalSet, sig);
	if (caught == -1)
	{
		cerr << SIGISMEMBER_FAIL << endl;
		exit(1);
	}

	return caught;

}
//resets the timer if threads were switched before the quantum
//expired. Assumes the timer was set before;
int Scheduler::resetTimer ()
{
	//stopping the timer
	if (setitimer (ITIMER_VIRTUAL,NULL, NULL))
	{
		cerr << SETITIMER_FAIL << endl;
		exit(1);
	}

	if (isPending (SIGVTALRM))
	{
		int sig_slot;
		sigset_t pendingSignalsSet;

		if (sigemptyset(&pendingSignalsSet))
		{
			cerr << SIGEMPTYSET_FAIL << endl;
			exit (1);
		}

		if (sigaddset (&pendingSignalsSet, SIGVTALRM))
		{
			cerr << SIGADDSET_FAIL << endl;
			exit (1);
		}
		//accepting the pending alarm signal
		if (sigwait (&pendingSignalsSet, &sig_slot))
		{
			cerr << SIGWAIT_FAIL<<  endl;
			exit(1);
		}
	}


	if(setitimer(ITIMER_VIRTUAL, &tv, NULL))
 	{
		cerr<< "system error: failed to set timer"<<endl;
		exit (1);
	}
	return OK;
}


//Prints debug data
void Scheduler::getDebugData (){
	cerr << "Running " << threads.running->id << endl;

	if (!threads.suspended.empty())
	{
		shared_ptr<Thread> temp = *threads.suspended.begin();
		//cout<< "Thread resumed: " << uthread_resume(temp->id) << endl; //Debug
		cerr << "readyThreads : \t";
			for (auto th : threads.readyQueue)
			{
				cerr <<" "<< th->id;
			}
			cerr << endl;

			int s;
			cin >> s;
	}

	cerr << "usedThreads : \t";
	for (auto thPair : usedThreads)
	{
		cerr <<" "<< thPair.first;
	}
	cerr << endl;

	cerr << "readyThreads : \t";
	for (auto th : threads.readyQueue)
	{
		cerr <<" "<< th->id;
	}
	cerr << endl;

	cerr << "Sleeping (id ,quanta left) : \t";
	for (auto th : threads.sleeping)
	{
		cerr <<"( "<< th->id  << " , " << th->sleepQuantoms << ")";
	}
	cerr << endl;

	cerr << "Suspended : \t";
			for (auto th : threads.suspended)
	{
		cerr <<" "<< th->id;
	}
	cerr << endl;
}

//Define scheduler schd (global)
Scheduler * schd = new Scheduler();

void timeHandler(int signum) {
	//cout << "Quantum has passed" << endl;
	int ret_val = sigsetjmp(schd->threads.running->env,1);
	  if (ret_val == 1) {
	      return;
	  }
	schd->quantumUpdate(signum);
	schd->startTimer();
	siglongjmp(schd->threads.running->env,1);
};


