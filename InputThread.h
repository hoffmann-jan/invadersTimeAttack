#ifndef INPUTTHREAD_H
#define INPUTTHREAD_H

#include <pthread.h>
#include <stdbool.h>

typedef struct 
{
    pthread_t pthread;
    pthread_mutex_t mutex; // fehler bei gleichzeitigem lesen und schreiben vermeiden
    bool stop;
    int key;

} InputThread;

InputThread *threadAlloc();
void threadStart(InputThread *thread);
void threadStop(InputThread *thread);
int threadKey(InputThread *thread);
void threadFree(InputThread *thread);

#endif