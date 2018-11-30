#include <stdlib.h>
#include <unistd.h>

#include "InputThread.h" //include also stdbool & pthread

#include <ncurses.h>
#define kbhit() true

InputThread *threadAlloc()
{
    InputThread *thread = (InputThread *)malloc(sizeof(InputThread));
    thread->pthread = 0;
    thread->key = 0;
    thread->stop = false;
 
    pthread_mutex_init(&thread->mutex,NULL); //mutex bereitstellen
    return thread;
}

//bedingte funktion für übergabe an pthread
static void *threadFunction(void *arg)
{
    InputThread *this = (InputThread *)arg; //übergeb mich
    while(!this->stop)
    {
        if(kbhit())
        {
            pthread_mutex_lock(&this->mutex); //zum schreiben locken, 
            const int ch = getch();
            this->key = ch;
            pthread_mutex_unlock(&this->mutex); //wieder verfügbar machen
        }
        usleep(500); //reduziere prozessorlast
    }
    return this;
}

void threadStart(InputThread *thread)
{
    pthread_create(&thread->pthread, NULL, threadFunction, thread);
}

void threadStop(InputThread *thread)
{
    thread->stop = true;
    pthread_join(thread->pthread, NULL); //warte bis alle threads fertig
}

void threadFree(InputThread *thread)
{
    free(thread);
}

int threadKey(InputThread *thread)
{
    pthread_mutex_lock(&thread->mutex);   //mutex locken, 
    int returnKey = thread->key;      //wert merken
    pthread_mutex_unlock(&thread->mutex);  //mutes freigeben
    return returnKey;
}

void releaseThreadKey(InputThread *thread)
{
    // pthread_mutex_lock(&thread->mutex);   //wieso funktoniert das lock hier nicht o.o
    thread->key = 0;
    // pthread_mutex_unlock(&thread->mutex);
}
