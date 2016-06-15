#ifndef THREAD_LIST_H
#define THREAD_LIST_H

#include "thread.h"
#include "list.h"

class ThreadList {
  public:
    ThreadList();
    ~ThreadList();
    int AddThread(Thread* t);
    Thread *GetThread(int i);
    void RemoveThread(Thread *t);

  private:
    List<Thread*> *threadList;
    int last;
    Lock *lock;
};

    
#endif
