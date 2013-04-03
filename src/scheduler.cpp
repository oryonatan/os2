#include "scheduler.h"
//For debug purposes: the number of allowed shared pointers owning the thread
#define COPIES_ALLOWED 3

int Scheduler::allocateID()
{
	for (int i = 1; i < MAX_THREAD_NUM; i++)
	{
		try
		{
			shared_ptr<Thread> temp = usedThreads.at(i);
		}

		catch (out_of_range&)
		{
			return i;
		}
	}

	return FAIL;

}

void Scheduler::quantumUpdate(int sig) {

	this->quanta++;
	for (shared_ptr<Thread> thread : threads.sleeping)
	{
		thread->sleepQuantoms--;
		if (thread->sleepQuantoms <= 0 )
		{
			moveThread (thread, READY);
		}

	}
	if(!threads.readyQueue.empty()){
		moveThread(move(threads.readyQueue.front()),RUNNING);
	}
}


shared_ptr<Thread> Scheduler::getThread(int tid) {

	shared_ptr<Thread> result = NULL;
	try
	{
		result = usedThreads.at(tid);
		return result;
	}
	catch (out_of_range)
	{
		return NULL;
	}
}



void Scheduler::eraseFromState (state originalState, shared_ptr<Thread> threadToErase)
{
	switch (originalState)
	{
		case RUNNING:
			threads.running.reset();
			break;
		case READY:
			threads.readyQueue.remove(threadToErase)
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
		cout << "new running thread " << th->id << ":" << th->env<< endl;
		if (threads.running) {
			moveThread(move(threads.running),READY);
		}
		threads.running = move(th);
}

void Scheduler::moveThread(shared_ptr<Thread> th, state newState) {

	state originalState = th->threadState;
	eraseFromState(originalState,th);

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
		terminateThread(move(th));
		break;
	default:
		break;
	}

	assert (th.use_count  <= COPIES_ALLOWED);
}

int Scheduler::spawnThread(thread_functor func)
{
	int id = allocateID;
	if (id == FAIL)
	{
		return FAIL;
	}

	shared_ptr<Thread> newThread = shared_ptr<Thread> (new Thread(func, id));
	usedThreads[id] = newThread;
	threads.readyQueue.push_back(move(newThread));

	return OK;

}

int  Scheduler::terminateThread(shared_ptr<Thread>& th) {
	if (th->threadState == RUNNING) {
		if (not threads.readyQueue.empty()) {
			cout << threads.readyQueue.size() << endl;
			moveThread(move(threads.readyQueue.front()), RUNNING);
		} else {
			cout << "ready list is empty" << endl;
		}
	}
	cleanEmptyThreads(th->threadState);
	//The pointer is invalidated , calling thread Dtor
	//TODO - remove the thread id from the control structures
}

void Scheduler::setup (int quantumLength)
{
	this->quantom_usecs = quantumLength;
	//TODO - add a new thread with ID 0 and make it running
	setMask();
	startTimer();

}

