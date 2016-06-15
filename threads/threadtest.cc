// threadtest.cc  
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Parts from Copyright (c) 2007-2009 Universidad de Las Palmas de Gran Canaria
//

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "port.h"
#include <unistd.h>
#include <string.h>


/* INICIO Productor Consumidor */
Lock *lock = new Lock ("Lock1");
Condition *empty = new Condition("empty", lock);
Condition *full = new Condition("full", lock);
int buffer[10];
int frees = 0;

void
Producer (void *name)
{
  for(;;){
    lock->Acquire();
    while(!(frees < 10))
      full->Wait();
    printf("Producing...\n");  
    buffer[frees] = 1;
    frees++;
    usleep(62500); //62500 / 1000000 = 0.0625 seg 
    printf("Produced. Free = %d\n", frees);  
    empty->Signal();
    lock->Release();
  }
}

void
Consumer (void *name)
{
  for(;;){
    lock->Acquire();
    while(!(frees > 0))
      empty->Wait();
    printf("Consuming...\n");  
    buffer[frees-1] = 0;
    frees--;
    usleep(62500); //62500 / 1000000 = 0.0625 seg 
    printf("Consumed. Free = %d\n", frees);  
    full->Signal();
    lock->Release();
  }
}

void
ProducerConsumerTest()
{
  DEBUG('t', "Entering ProducerConsumerTest...");
  
  const char *threadname[2] = {"Producer" , "Consumer"};
  Thread *ProducerT = new Thread (threadname[0], 0);
  ProducerT->Fork(Producer, (void*)threadname[0]);
  Consumer((void*) threadname[1]);
}
/* FIN Productor Consumidor */

/* INICIO Port */

Port *port = new Port("Port1");

void
Receiver (void *name)
{
  int *buff = new int;
  for(;;) {
    port->Receive(buff);
    printf("%s received: %d\n", (char*)name, *buff);
    fflush(stdout);
    usleep(500000);
  }
}

void
Sender (void *name)
{
  int i;
  for (i = 0; ; i++) {
    currentThread->Yield();
    printf("%s sending: %d\n", (char*)name, i);
    fflush(stdout);
    port->Send(i);
    usleep(500000);
  }
}


void
PortTest()
{
  const char *threadname[3] = {"Receiver", "Sender1", "Sender2"};
  Thread *sender1 = new Thread (threadname[1], 0);
  Thread *sender2 = new Thread (threadname[2], 0);
  sender1->Fork(Sender, (void*)threadname[1]);
  sender2->Fork(Sender, (void*)threadname[2]);
  Receiver((void*) threadname[0]);
}

/* FIN Port */

/* INICIO Join */

void
ThreadJoinableTrue(void *name)
{
  printf("%s: init, begin sleep.\n", (char*)name);
  sleep(5);
  printf("%s: sleep ends, finishing.\n", (char*)name);
}

void
ThreadJoinableFalse(void *joinableThread)
{
  Thread *join = (Thread *)joinableThread;
  printf("%s: init, exec join.\n", currentThread->getName());
  join->Join();
  printf("%s: join returned.\n", currentThread->getName());
}

void
JoinTest()
{
  const char *threadname = "thread1";
  Thread *thread1 = new Thread (threadname, 0);
  thread1->Fork(ThreadJoinableTrue, (void*)threadname);
  ThreadJoinableFalse((void*)thread1);
}

/* FIN Join */

/* INICIO MultiQueues */
/* SIN RS */
Lock *MQLock = new Lock("MQLock");

void
MQThreadTest(void *_name)
{
  char *name = (char*) _name;
  int i;
  MQLock->Acquire(); /* Low will insta acquire. Others won't, but we don't let them yield if they acquire then */
  if (!strcmp(name, "Low"))
    currentThread->Yield(); /* Goes back to main to continue setting, if it is Low */
    
  for(i = 0; i < 3; i++){
    printf("%s : Running... With priority %d\n", name, currentThread->GetEffPriority());
    usleep(62500);
    currentThread->Yield(); /* Goes back to highest priority thread */
  }
  MQLock->Release();
  printf("%s : Finished\n", name);
}

void
MQThreadTestMedium(void *_name)
{
  char *name = (char*) _name;
  for (;;){
    printf("%s : Running...\n", name);
    usleep(62500);
    currentThread->Yield(); /* let others run... if they can't imp doesn't work. */
  }
}



void
MultiQueuesTest()
{
  currentThread->SetEffPriority(5); /* set ourselves to highest priority. */
              /* Priorities:     1         2          3        4       5 */  
  const char *threadname[4] = {"Low", "MediumLow", "Medium", "High" };
  Thread *thread;
  
  int i;
  for(i=0; i < 4; i++){
    if (i == 2) /* We set up Medium after, main will be Medium*/
      continue;
    thread = new Thread(threadname[i], i+1);
    thread->Fork(MQThreadTest,(void*)threadname[i]);
    currentThread->Yield(); /* let them set up */
  }
  
  currentThread->SetEffPriority(3); /* we set ourselves to Medium Priority */
  MQThreadTestMedium((void*)threadname[2]); /* we run. */
}
  
/* FIN MultiQueues */


//----------------------------------------------------------------------
// SimpleThread
// 	Loop 10 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"name" points to a string with a thread name, just for
//      debugging purposes.
//----------------------------------------------------------------------


Semaphore *f;
void
SimpleThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.

    for (int num = 0; num < 10; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread %s looped %d times\n", threadName, num);
	//interrupt->SetLevel(oldLevel);
        currentThread->Yield();
    }
    
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between several threads, by launching
//	ten threads which call SimpleThread, and finally calling 
//	SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    char *threadname = new char[128];
    strcpy(threadname,"Hilo 1");
    Thread* newThread = new Thread (threadname, 0);
    newThread->Fork (SimpleThread, (void*)threadname);
    
    SimpleThread( (void*)"Hilo 0");
    
}

