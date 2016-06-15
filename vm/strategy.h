#ifndef STRATEGY_H
#define STRATEGY_H

#include "list.h"

/* Dedicado a Maxi */
class Strategy {
  public:
    virtual int Next(void *_data) = 0;
    virtual void Add(void *_data) = 0;
};

class FIFO: public Strategy {
  public:
    FIFO();
    ~FIFO();
    int Next(void *);
    void Add(void *_data);
  private:
    List<int> *queue;
};

class LRU: public Strategy {
  public:
    LRU();
    ~LRU();
    int Next(void*);
    void Add(void *_data);
  private:
    List<int> *queue;
};

class ESC: public Strategy {
  public:
    ESC(int max);
    //~AMR();
    int Next(void *_data);
    void Add(void *);
  private:
    int npp;
    int victim;
};


#endif
