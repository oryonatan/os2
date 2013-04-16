
CFLAGS = -std=c++0x -Wall -I.


all: libuthreads.a

libuthreads.a:  scheduler.o thread.o uthreads.o  scheduler.o thread.o
	ar rcu libuthreads.a uthreads.o scheduler.o thread.o
	ranlib libuthreads.a


uthreads.o: uthreads.cpp uthreads.h scheduler.h 
	g++ uthreads.cpp $(CFLAGS) -c

scheduler.o: scheduler.cpp scheduler.h  thread.h
	g++ scheduler.cpp $(CFLAGS) -c

thread.o: thread.cpp thread.h 
	g++ thread.cpp $(CFLAGS) -c

clean:
	rm -rf *.o *.a *~ Tester
