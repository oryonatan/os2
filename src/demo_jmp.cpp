#ifndef _demo_jmp_c
#define _demo_jmp_c

/*
 * sigsetjmp/siglongjmp demo program.
 * Hebrew University OS course.
 * Questions: os@cs.huji.ac.il
 */


//DEBUG
#include <random>


#include "uthreads.h"
#include "scheduler.h"
#include "thread.h"
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND 1000000
#define STACK_SIZE 4096

extern Scheduler * schd;
//Scheduler * schd;






//
///* A translation is required when using an address of a variable.
//   Use this as a black box in your code. */
//address_t translate_address(address_t addr)
//{
//    address_t ret;
//    asm volatile("xor    %%fs:0x30,%0\n"
//		"rol    $0x11,%0\n"
//                 : "=g" (ret)
//                 : "0" (addr));
//    return ret;
//}







void f(void)
{
	printf("in f \n");
  int i = 0;
  while(1){
    ++i;
    if ( i == 10001)
        {
        	int tid = uthread_get_tid();
        	uthread_suspend(tid);
        }
    if (i % 300000000== 0) {

        printf("in f (%d)\n",i);
//    	schd->getDebugData();
        }
//    if (i % 3 == 0) {
//      printf("f: switching\n");
//      switchThreads();
//      schdSwitchThreads();
//    }
//    usleep(SECOND);
  }
}

void g(void)
{
	printf("in g \n");
  int i = 0;
  while(1){
    ++i;

    if (i % 300000000 == 0) {
        printf("in g (%d)\n",i);
//        schd->getDebugData();
//        cout << "suspending 1";
//        uthread_suspend(1);
//        schd->getDebugData();
        }
//    if (i % 5 == 0) {
//      printf("g: switching\n");
//      switchThreads();
//      schdSwitchThreads();
//    }
//    usleep(SECOND);
  }
}
void h(void)
{
	cout<<"h"<<endl;
  int i = 0;
  while(1){
    ++i;
    if (i % 300000000 == 0) {
    printf("in h (%d) sleep h \n",i);
    uthread_sleep(10);
    printf("sleep end");
    }
//    if (i % 4 == 0) {
//      printf("h: switching\n");
//      switchThreads();
//      schdSwitchThreads();
//    }
//    usleep(SECOND);
  }
}

void m(void) {
	cout << "m" << endl;
	int i = 0;
	while (1) {
		++i;
		if (i % 300000000 == 0) {
			printf("in m(%d)\n", i);
//			schd->getDebugData();
//			cout << "resume 1" << endl;
//			uthread_resume(1);
//			schd->getDebugData();
		}
	}
}




int main(void){

	//initial test :
	//create two threads
	//switch between them on timely manner
//	schd = new Scheduler();
	uthread_init(1000000);
	uthread_spawn(f);
//	uthread_spawn(g);
	uthread_spawn(h); // sleepy
//	uthread_spawn(m);

	int i;
	for ( i = 0; i > -1; i++ )
	{
		if (i == 1000000004)
		{
			if (uthread_spawn (f))
			{
				cout << "ran out of thread ids" << endl;
				return 0;
			}
		}

		if (i % 1000000 == 0)
		{
			schd->getDebugData();
		}

		if (i == 1000001)
		{
			if (!uthread_suspend (0))
			{
				int s;
				cout << "main thread suspending itself didn't cause an error" << endl;
				cin >> s;
			}
		}

		if (i == 10002)
		{
			if (!uthread_sleep(5))
			{
				int s;
				cout << "main thread sleeping itself didn't cause an error" << endl;
				cin >> s;
			}
		}

	}

	return 0;
}
#endif
