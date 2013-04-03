#include "uthreads.h"
#include "scheduler.h"

Scheduler * sched = new Scheduler();

int uthread_init(int quantum_usecs) {
	sched->setup (quantum_usecs);
}

int uthread_spawn(void (*f)(void)) {
	///TODO should initialize with a pointer to function
	unique_ptr<Thread> th(new Thread(f));
	sched->moveThread(move(th),READY);
	//TODO check return value
	return 0;
}

int uthread_terminate(int tid) {
	//TODO implement by TID
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
