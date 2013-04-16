/*
 * tester.cpp
 *
 *  Created on: Mar 27, 2013
 *      Author: tamary
 */


#include <unistd.h>
#include "uthreads.h"
#include "iostream"
#define INTERVAL 100000000
using namespace std;

void foo() {
	int counter = 0;
	while(true) {

//		static bool temp = true;
//		if (uthread_get_tid() == 19 && temp) {
//			temp = false;
//			uthread_suspend(5);
//		}
//		else if (uthread_get_tid() == 6) {
//			uthread_resume(5);
//		}

		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! foo , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		counter++;
	}
}

void bar() {
	int counter = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! bar , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		counter++;
	}
}

void something() {
	int counter = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! something , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		counter++;
	}
}

void barSuspend() {
	int counter = 0;
	int unsusCounter = 0;
	bool susSwitch = false;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! bar , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if(unsusCounter == INTERVAL*5 ) {
			if (!susSwitch) {
				cout << "[tester] return value of suspend: "<<uthread_suspend(1) << endl;
				susSwitch = true;
			}
		}

		if (unsusCounter == INTERVAL*10 ) {
			cout << "[test2] trying to resume"<< endl;
			cout << "[tester] return value of resume: "<<uthread_resume(1)<< endl;
			unsusCounter = 0;
			susSwitch = false;
		}
		unsusCounter ++;
		counter++;
	}
}

void selfSuspendFoo() {
	int counter = 0;
	int self_suspend_ctr = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! foo , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if (self_suspend_ctr == INTERVAL*10) {
			self_suspend_ctr = 0;
			cout << "[test3] trying to suspend "<< uthread_get_tid() << endl;
			cout << "[tester] return value of suspend: "<<uthread_suspend(uthread_get_tid())<<endl;
			cout.flush();
		}
		self_suspend_ctr++;
		counter++;
	}
}

void barResumeOtherThread() {
	int counter = 0;
	int unsusCounter = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! bar , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if (unsusCounter == INTERVAL*20 ) {
			cout << "[test3] trying to resume 1"<< endl;
			cout << "[tester] return value of resume: "<<uthread_resume(1) <<endl;
			cout.flush();
			unsusCounter = 0;
		}
		unsusCounter ++;
		counter++;
	}
}

void barTerminateSelf() {
	int counter = 0;
	int killCounter = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! barTermSelf , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if (killCounter == INTERVAL*5 ) {
			cout << "[test4] farewell sweet world!!! (self terminate)"<< endl;
			cout << "[tester] return value of terminate: "<< uthread_terminate(uthread_get_tid())<<endl;
			killCounter = 0;
		}
		killCounter ++;
		counter++;
	}
}

void barSleep() {
	int counter = 0;
	int sleepCounter = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread!, thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if (sleepCounter == INTERVAL*10 ) {
			cout << "[test7] I'm going to sleep"<< endl;
			cout << "[tester] return value of sleep: "<< uthread_sleep(7)<< endl;
			sleepCounter = 0;
		}
		sleepCounter ++;
		counter++;
	}
}

void fooCreatesNewThread() {
	int counter = 0;
	int create_counter = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! foo , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if (create_counter == INTERVAL*20) {
			create_counter = 0;
			cout << "[tester] return value of spawn: "<< uthread_spawn(foo)<<endl;
		}
		create_counter++;
		counter++;
	}
}

void fooTerminatesMainThread() {
	int counter = 0;
	int kill_counter = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! foo , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if (kill_counter == INTERVAL*5) {
			kill_counter = 0;
			cout << "[tester] return value of terminate: "<< uthread_terminate(0)<<endl;
			cout.flush();
		}
		kill_counter++;
		counter++;
	}
}

void barTerminateOther() {
	int counter = 0;
	int killCounter = 0;
	bool to_kill = true;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread! barTermOther , thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if (to_kill == true && killCounter == INTERVAL*5) {
			to_kill = false;
			cout << "[test6] farewell sucker!!! (terminate 1)"<< endl;
			cout << "[tester] return value of terminate: "<< uthread_terminate(1)<< endl;
			killCounter = 0;
		}
		killCounter ++;
		counter++;
	}
}

void barQuantum() {
	int counter = 0;
	int quantumCounter = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread!, bar at your service, thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if (quantumCounter == INTERVAL*3 ) {
			cout << "[test8] (bar) I'm bar, counting quantums: {" << uthread_get_quantums(uthread_get_tid()) << "}"<< endl;
			quantumCounter = 0;
		}
		quantumCounter ++;
		counter++;
	}
}

void fooQuantum() {
	int counter = 0;
	int quantumCounter = 0;
	while(true) {
		if (counter == INTERVAL) {
			counter = 0;
			cout << "[tester] ";
			cout << "I'm a thread!, foo at your service, thread number : ["<< uthread_get_tid() <<"]" << endl;
		}
		if (quantumCounter == INTERVAL*3 ) {
			cout << "[test8] (foo) I'm foo, counting quantums: {" << uthread_get_quantums(uthread_get_tid()) << "}"<< endl;
			cout << "[test8] (foo) Total quantums: {" << uthread_get_total_quantums() << "}"<< endl;
			quantumCounter = 0;
		}
		quantumCounter ++;
		counter++;
	}
}

// basic switch test
void test1() {
	cout << "[tester] Test 1: basic test" <<endl;

	if (uthread_init(1000000) != 0) {
		cout << "[tester] ohh snap! init returned an error code" <<endl;
	}
	else {
		cout <<"[tester] spawn should not return -1: " <<uthread_spawn(foo)<<endl;
		cout <<"[tester] spawn should not return -1: " <<uthread_spawn(bar)<<endl;
	}
}

// suspend test
void test2() {
	cout << "[tester] Test 2: suspend test and unsuspend" <<endl;
	uthread_init(1000000);
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(foo) <<endl;
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(barSuspend) <<endl;
}

// suspend running thread
void test3() {
	cout << "[tester] Test 3: suspend running thread and unsuspend it" <<endl;
	uthread_init(1000000);
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(selfSuspendFoo) <<endl;
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(barResumeOtherThread) <<endl;
}

// terminate self. create new thread to check that number 2 is used again.
void test4() {
	cout << "[tester] Test 4: thread terminates itself" <<endl;
	uthread_init(1000000);
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(fooCreatesNewThread) <<endl;
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(barTerminateSelf) <<endl;
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(something) <<endl;
}

// try to terminate main
void test5() {
	cout << "[tester] Test 5: thread tries to terminates main thread" <<endl;
	uthread_init(1000000);
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(fooTerminatesMainThread) <<endl;
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(bar) <<endl;
}

// try to terminate other thread
void test6() {
	cout << "[tester] Test 6: thread terminates other thread" <<endl;
	uthread_init(1000000);
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(foo) <<endl;
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(barTerminateOther) <<endl;
}

// sleep test
void test7() {
	cout << "[tester] Test 7: sleep test" <<endl;
	uthread_init(1000000);
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(barSleep) <<endl;
}

// quantum test
void test8() {
	cout << "[tester] Test 8: quantum test" <<endl;
	uthread_init(1000000);
	cout << "[tester] Total quantums: " << uthread_get_total_quantums() << endl;
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(fooQuantum) <<endl;
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(barQuantum) <<endl;
}

// suspend test with large quantum so we can see quantum stopped before it is up
void test9() {
	cout << "[tester] Test 9: suspend running thread and unsuspend it" <<endl;
	uthread_init(10000000);
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(selfSuspendFoo) <<endl;
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(barResumeOtherThread) <<endl;
}

// test more than 100 threads
void test10() {
	uthread_init(1000000);
	for (int i = 1; i < 20; i++) {
		cout <<"[tester] spawn should not return -1: " <<uthread_spawn(foo) <<endl;
	}
	usleep(4000000);
//	cout << "(test10) after sleep" << endl;
//
//	for (int i = 19; i > 0; i--) {
//		cout <<"[tester] spawn should not return -1: " <<uthread_terminate(i) <<endl;
//	}
//	for (int i = 1; i < 100; i++) {
//		cout <<"[tester] spawn should not return -1: " <<uthread_spawn(foo) <<endl;
//	}
//	cout <<"[tester] spawn should return -1: " <<uthread_spawn(foo) <<endl;
//	cout <<"[tester] return value of terminate: "<< uthread_terminate(5)<<endl;
//	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(foo) <<endl;
//	cout <<"[tester] spawn should return -1: " <<uthread_spawn(foo) <<endl;

}

// valgrind
void test11() {
	cout << "[tester] Test 11: terminate main thread" <<endl;
	uthread_init(1000000);
	cout <<"[tester] spawn should not return -1: " <<uthread_spawn(foo) <<endl;
	uthread_terminate(0);
}

//
//int main(){
////	test1();
////	test2();
////	test3();
//	test4();
////	test5();
////	test6();
////	test7();
////	test8();
////	test9();
////	test10();
////	test11();
////	int counter = 0, quantumCounter = 0;
//	while(true) {
//////		if (counter == INTERVAL) {
//////			counter = 0;
//////			cout << "[tester] ";
//////			cout << "I'M SEXY AND I KNOW IT, thread number : ["<< uthread_get_tid() <<"]" << endl;
//////		}
//////		if (quantumCounter == INTERVAL*3 ) {
//////			cout << "[test8] (MAIN) I'm MAIN, counting quantums: {" << uthread_get_quantums(uthread_get_tid()) << "}"<< endl;
//////			cout << "[test8] (MAIN) Total quantums: {" << uthread_get_total_quantums() << "}"<< endl;
//////			quantumCounter = 0;
////		}
////		quantumCounter ++;
////		counter++;
//	}
////	return 0;
//}



