#include "threadlist.h"
#include "synch.h"

ThreadList::ThreadList() {
  threadList = new List<Thread*>;
  last = 0;
  lock = new Lock("ThreadList Lock");
}

ThreadList::~ThreadList() {
  delete threadList;
  delete lock;
}

int ThreadList::AddThread(Thread* t) {
  lock->Acquire();
  threadList->SortedInsert(t, last);
  int id = last++;
  lock->Release();
  return id;
}


Thread *ThreadList::GetThread(int i) {
  return threadList->Seek(i);
}

void ThreadList::RemoveThread(Thread *t) {
  if (t != NULL) {
    threadList->ItemRemove(t);
  }
}

