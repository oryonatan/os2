#include "uthreads.h"
#include "scheduler.h"

Scheduler * sched = new Scheduler();

int uthread_init(int quantum_usecs) {
	//TODO can it fail?
	if (sched->setTimeInterval(quantum_usecs))
		return OK;
	return FAIL;
}

int uthread_spawn(void (*f)(void)) {
	///TODO should initialize with a pointer to function
	unique_ptr<Thread> th(new Thread());
	sched->moveThread(move(th),READY);
	//TODO check return value
	return 0;
}

int uthread_terminate(int tid) {
	//TODO implement by TID
	return 0;
}

int uthread_suspend(int tid) {
	return 0;
}

int uthread_resume(int tid) {
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
