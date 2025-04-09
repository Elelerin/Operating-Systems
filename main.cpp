#include <iostream>
#include <pthread.h>
#include <mutex>
#include <random>
#include <unistd.h>
class ringBuffer;

enum QUEUESTATE{
    IDLE  = 0,
    READY = 1,
    BUSY  = 2
};

pthread_mutex_t lockGuard = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t highCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t lowCond = PTHREAD_COND_INITIALIZER;

//Technically, I could use some form of atomic to do this more efficiently if we have a looping ring
//And multiple threads running concurrently.
class ringBuffer{
    public:
        int32_t     *values, *head, *tail;
        int32_t     valueCount, maxSize;
        QUEUESTATE status = IDLE;


    //----------------------//
    //Constructor / Destructor
    ringBuffer(int circumference){
        values  = (int32_t*)malloc(sizeof(int32_t) * circumference);
        valueCount   = 0;
        maxSize = circumference;
        head    = values;
        tail    = values;
        status  = IDLE;
    }



    ~ringBuffer(){
        free(values);
    }

    //Will operate in N time;
    void remove(){
        pthread_mutex_lock(&lockGuard);

        //creates a wait condition until it is unlocked.
        //this also prevents the over-write of head, I think.
        while (valueCount == 0){
            pthread_cond_wait(&lowCond, &lockGuard);
        }

        //Check for wrap
        *tail = toInsert;
        head--;

        printf("Removed %d, count = %d\n", toInsert, valueCount);
        if(tail == 0){
            tail = values + valueCount;
        }

        if (valueCount == 0) {
            pthread_cond_signal(&lowCond); // Wake producer
        }
        valueCount--;

        pthread_mutex_unlock(&lockGuard);
    }

    void insert(int32_t toInsert){
        pthread_mutex_lock(&lockGuard);

        //creates a wait condition until it is unlocked.
        //this also prevents the over-write of head, I think.
        while (valueCount == maxSize){
            pthread_cond_wait(&highCond, &lockGuard);
        }

        //Check for wrap
        *head = toInsert;
        head++;

        printf("Inserted %d, count = %d\n", toInsert, valueCount);
        if(head == values + (maxSize)){
            head = values;
        }

        if (valueCount == maxSize - 1) {
            pthread_cond_signal(&highCond); // Wake producer
        }
        valueCount++;

        pthread_mutex_unlock(&lockGuard);
    }
};

class producer{
    public:
    pthread_mutex_t* mutexGuard;
    pthread_cond_t* cond;
    ringBuffer* buffer;
    pthread_t thread;
    int32_t operations;

    producer(pthread_mutex_t* inGuard, pthread_cond_t* inCond, ringBuffer* inBuffer, const int32_t timesToPerform = 1){
        mutexGuard = inGuard;
        cond = inCond;
        buffer = inBuffer;
        operations = timesToPerform;
        if (pthread_create(&thread, NULL, producer::produce, this) != 0) {
            perror("Failed to create producer thread");
            exit(1);
        }
    }

    ~producer(){
        pthread_join(thread, NULL);
    }

    static void* produce(void* args){
        producer* p = (producer*)args;

        for(int i = 0; i < p->operations; i++){
            p->buffer->insert(rand() % 1000);
            p->thread->sleep(rand() % 1000);
        }

        return NULL;
    }
};

class consumer{
    public:
    pthread_mutex_t* mutexGuard;
    pthread_cond_t* cond;
    ringBuffer* buffer;
    pthread_t thread;
    int32_t operations;

    consumer(pthread_mutex_t* inGuard, pthread_cond_t* inCond, ringBuffer* inBuffer){
        mutexGuard = inGuard;
        cond = inCond;
        inBuffer = buffer;
    }


    consumer(pthread_mutex_t* inGuard, pthread_cond_t* inCond, ringBuffer* inBuffer, const int32_t timesToPerform = 1){
        mutexGuard = inGuard;
        cond = inCond;
        buffer = inBuffer;
        operations = timesToPerform;
        if (pthread_create(&thread, NULL, consumer::consume, this) != 0) {
            perror("Failed to create consumer thread");
            exit(1);
        }
    }

    static void* consume(void* args){
        consumer* c = (consumer*)args;
        for(int i = 0; i < c->operations; i++){
            c->buffer->remove();
            c->thread->sleep(rand() % 1000);
        }

        return NULL;
    }
};



int main(){
    ringBuffer ring(100);

    sleep(1);
    producer p(&lockGuard, &highCond, &ring, 55);
    producer q(&lockGuard, &highCond, &ring, 55);
    sleep(1);


    return 0;
}
