#ifndef _demo_jmp_c
#define _demo_jmp_c

/*
 * sigsetjmp/siglongjmp demo program.
 * Hebrew University OS course.
 * Questions: os@cs.huji.ac.il
 */

#include "uthreads.h"
#include "scheduler.h"
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND 1000000
#define STACK_SIZE 4096

//Scheduler * schd;

void timer_handler(int);

char stack1[STACK_SIZE];
char stack2[STACK_SIZE];
char stack3[STACK_SIZE];

sigjmp_buf env[2];

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7



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

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif

void switchThreads(void)
{
  static int currentThread = 0;

  int ret_val = sigsetjmp(env[currentThread],1);
  printf("SWITCH: ret_val=%d\n", ret_val);
  if (ret_val == 1) {
      return;
  }
  currentThread = 1 - currentThread;
  siglongjmp(env[currentThread],1);
}

void schdSwitchThreads(void)
{
  int ret_val = sigsetjmp(schd->threads.running->env,1);

  printf("SWITCH: ret_val=%d\n", ret_val);
  if (ret_val == 1) {
      return;
  }
//  schd->updateRunning();
  siglongjmp(schd->threads.running->env,1);

}


void f(void)
{
	printf("in f \n");
  int i = 0;
  while(1){
    ++i;
    if (i % 100000000== 0) {
        printf("in f (%d)\n",i);
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
    if (i % 100000000 == 0) {
        printf("in g (%d)\n",i);
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
    if (i % 100000000 == 0) {
    printf("in h (%d)\n",i);
    }
//    if (i % 4 == 0) {
//      printf("h: switching\n");
//      switchThreads();
//      schdSwitchThreads();
//    }
//    usleep(SECOND);
  }
}


void setup(void)
{

  address_t sp, pc;

  sp = (address_t)stack1 + STACK_SIZE - sizeof(address_t);
  pc = (address_t)f;
  sigsetjmp(env[0], 1);
  (env[0]->__jmpbuf)[JB_SP] = translate_address(sp);
  (env[0]->__jmpbuf)[JB_PC] = translate_address(pc);
  sigemptyset(&env[0]->__saved_mask);

  sp = (address_t)stack2 + STACK_SIZE - sizeof(address_t);
  pc = (address_t)g;
  sigsetjmp(env[1], 1);
  (env[1]->__jmpbuf)[JB_SP] = translate_address(sp);
  (env[1]->__jmpbuf)[JB_PC] = translate_address(pc);
  sigemptyset(&env[1]->__saved_mask);
}
void schdSetup()
{
//	address_t sp, pc;
//
//	unique_ptr<Thread> fThread = (unique_ptr<Thread>)new Thread();
//	sp = (address_t) fThread->stack + STACK_SIZE - sizeof(address_t);
//	pc = (address_t) f;
//	sigsetjmp(fThread->env, 1);
//	(fThread->env->__jmpbuf)[JB_SP] = translate_address(sp);
//	(fThread->env->__jmpbuf)[JB_PC] = translate_address(pc);
//	sigemptyset(&fThread->env->__saved_mask);
//	schd->moveThread(move(fThread),RUNNING);
//
//	unique_ptr<Thread> gThread = (unique_ptr<Thread>)new Thread();
//	sp = (address_t) gThread->stack + STACK_SIZE - sizeof(address_t);
//	pc = (address_t) g;
//	sigsetjmp(gThread->env, 1);
//		(gThread->env->__jmpbuf)[JB_SP] = translate_address(sp);
//	(gThread->env->__jmpbuf)[JB_PC] = translate_address(pc);
//	sigemptyset(&gThread->env->__saved_mask);
//	schd->moveThread(move(gThread),READY);
//
//	unique_ptr<Thread> hThread = (unique_ptr<Thread>)new Thread();
//	sp = (address_t) hThread->stack + STACK_SIZE - sizeof(address_t);
//	pc = (address_t) h;
//	sigsetjmp(hThread->env, 1);
//	(hThread->env->__jmpbuf)[JB_SP] = translate_address(sp);
//	(hThread->env->__jmpbuf)[JB_PC] = translate_address(pc);
//	sigemptyset(&hThread->env->__saved_mask);
//	schd->moveThread(move(hThread),READY);
}

void timer_handler(int sig)
{
	cout<<"time!"<<endl;
	schdSwitchThreads();
}
int main(void){
//  setup();
//  schd = new Scheduler(2);
//  schdSetup();
//  struct sigaction act,oact;
//  sigemptyset(&act.sa_mask);
//  act.sa_flags = 0;
//  act.sa_handler = timer_handler;
//  sigaction(SIGVTALRM,&act,&oact);
//
//  struct itimerval tv;
//  tv.it_value.tv_sec = 2;  /* first time interval, seconds part */
//  tv.it_value.tv_usec = 0; /* first time interval, microseconds part */
//  tv.it_interval.tv_sec = 2;  /* following time intervals, seconds part */
//  tv.it_interval.tv_usec = 0; /* following time intervals, microseconds part */
//  setitimer(ITIMER_VIRTUAL, &tv, NULL);
//  siglongjmp(schd->threads.running->env, 1);

}
#endif
