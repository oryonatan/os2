#include "scheduler.h"

void Scheduler::updateRunning() {
	moveThread(move(threads.readyQueue.front()),RUNNING);
}

bool Scheduler::setTimeInterval(int quantom_usecs) {
	this->quantom_usecs = quantom_usecs;
}

void Scheduler::cleanEmptyThreads(state originalState= NONE_SPECIFIED) {
	switch (originalState) {
	case RUNNING:
		//nothing to do here
		break;
	case READY:
		threads.readyQueue.remove(NULL);
		break;
	case SUSPENDED:
		threads.suspended.erase(NULL);
		break;
	case SLEEPING:
		threads.sleeping.erase(NULL);
		break;
	case NONE_SPECIFIED:
		threads.readyQueue.remove(NULL);
		threads.suspended.erase(NULL);
		threads.sleeping.erase(NULL);
		break;
	default:
		break;
	}
}

void Scheduler::setRunningThread(unique_ptr<Thread> th) {
		cout << "new running thread " << th->id << ":" << th->env<< endl;
		if (threads.running) {
			moveThread(move(threads.running),READY);
		}
		threads.running = move(th);
}

void Scheduler::moveThread(unique_ptr<Thread> th, state newState) {
	state originalState = th->threadState;
	cleanEmptyThreads(originalState);

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
}

void Scheduler::terminateThread(unique_ptr<Thread> th) {
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
}
