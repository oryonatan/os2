
CFLAGS = -std=c++0x -Wall -I.


all: libuthreads.a Tester

libuthreads.a:  scheduler.o thread.o uthreads.o  scheduler.o thread.o
	ar rcu libuthreads.a uthreads.o scheduler.o thread.o
	ranlib libuthreads.a

Tester: johnny_tester.o libuthreads.a
	g++ johnny_tester.o $(CFLAGS) -L. -luthreads -o Tester

johnny_tester.o: johnny_tester.cpp uthreads.h
	g++ johnny_tester.cpp $(CFLAGS) -c

uthreads.o: uthreads.cpp uthreads.h scheduler.h 
	g++ uthreads.cpp $(CFLAGS) -c

scheduler.o: scheduler.cpp scheduler.h address_translation.h thread.h
	g++ scheduler.cpp $(CFLAGS) -c

thread.o: thread.cpp thread.h 
	g++ thread.cpp $(CFLAGS) -c

clean:
	rm -rf *.o *.a *~ Tester
