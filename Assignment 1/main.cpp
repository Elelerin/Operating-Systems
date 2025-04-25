#include <iostream>
#include <pthread.h>
#include <mutex>
#include <random>
#include <unistd.h>
#include <vector>
#include <chrono>

class ringBuffer;

enum QUEUESTATE {
    IDLE = 0,
    READY = 1,
    BUSY = 2
};

pthread_mutex_t lockGuard = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dataAvailable = PTHREAD_COND_INITIALIZER;  // Signals consumers
pthread_cond_t spaceAvailable = PTHREAD_COND_INITIALIZER; // Signals producers
int runningProducers = 0;

class ringBuffer {
public:
    int32_t *values, *head, *tail;
    int32_t valueCount, maxSize;
    QUEUESTATE status = IDLE;

    ringBuffer(int circumference) {
        values = (int32_t*)malloc(sizeof(int32_t) * circumference);
        if (!values) {
            perror("Failed to allocate memory for ring buffer");
            exit(1);
        }
        valueCount = 0;
        maxSize = circumference;
        head = values;
        tail = values;
        status = IDLE;
    }

    ~ringBuffer() {
        free(values);
    }

    void remove() {
        pthread_mutex_lock(&lockGuard);

        while (valueCount == 0) {
            pthread_cond_wait(&dataAvailable, &lockGuard);
        }

        printf("Removed %d, count = %d\n", *tail, valueCount);

        valueCount--;
        if (tail == values + maxSize - 1) {
            tail = values;
        } else {
            tail++;
        }

        pthread_cond_signal(&spaceAvailable);

        pthread_mutex_unlock(&lockGuard);
    }

    void insert(int32_t toInsert) {
        pthread_mutex_lock(&lockGuard);

        while (valueCount == maxSize) {
            pthread_cond_wait(&spaceAvailable, &lockGuard);
        }

        *head = toInsert;
        printf("Inserted %d, count = %d\n", toInsert, valueCount);
        valueCount++;

        if (head == values + maxSize - 1) {
            head = values;
        } else {
            head++;
        }

        pthread_cond_signal(&dataAvailable);

        pthread_mutex_unlock(&lockGuard);
    }

    void count(int& countToReturn){
        pthread_mutex_lock(&lockGuard);
        countToReturn = valueCount;
        pthread_mutex_unlock(&lockGuard);
    }
};

class producer {
public:
    pthread_mutex_t* mutexGuard;
    pthread_cond_t* cond;
    ringBuffer* buffer;
    pthread_t thread;
    int32_t operations;
    pthread_cond_t completeCond = PTHREAD_COND_INITIALIZER;

    producer(pthread_mutex_t* inGuard, pthread_cond_t* inCond, ringBuffer* inBuffer, const int32_t timesToPerform = 1) {
        mutexGuard = inGuard;
        cond = inCond;
        buffer = inBuffer;
        operations = timesToPerform;
        runningProducers++;
    }

    void start() {
        if (pthread_create(&thread, NULL, producer::produce, this) != 0) {
            perror("Failed to create producer thread");
            exit(1);
        }
    }

    pthread_cond_t* Complete(){
        return &completeCond;
    }

    ~producer() {
        pthread_join(thread, NULL);
    }

    static void* produce(void* args) {
        producer* p = (producer*)args;
        for (int i = 0; i < p->operations; i++) {
            p->buffer->insert(rand() % 1000);
            uint64_t waitTime = (rand() % 1000) + 1;
            auto startTime = std::chrono::steady_clock::now();
            while(true){
                if(std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - startTime).count() >= waitTime){
                    break;
                }
            }
        }
        runningProducers--;
        pthread_cond_signal(&p->completeCond);
        return NULL;
    }
};

class consumer {
public:
    pthread_mutex_t* mutexGuard;
    pthread_cond_t* cond;
    ringBuffer* buffer;
    pthread_t thread;
    bool die = false;

    consumer(pthread_mutex_t* inGuard, pthread_cond_t* inCond, ringBuffer* inBuffer) {
        mutexGuard = inGuard;
        cond = inCond;
        buffer = inBuffer;
    }

    void start() {
        if (pthread_create(&thread, NULL, consumer::consume, this) != 0) {
            perror("Failed to create consumer thread");
            exit(1);
        }
    }

    void stop() {
        pthread_mutex_lock(mutexGuard);
        die = true;
        pthread_cond_broadcast(cond);
        pthread_mutex_unlock(mutexGuard);
    }

    ~consumer() {
        pthread_join(thread, NULL);
    }

    static void* consume(void* args) {
        consumer* c = (consumer*)args;
        while (!c->die) {
            c->buffer->remove();
            uint64_t waitTime = (rand() % 1000) + 1;
            auto startTime = std::chrono::steady_clock::now();
            while(true){
                if(std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - startTime).count() >= waitTime){
                    break;
                }
            }
        }
        return NULL;
    }
};

int main() {
    srand(time(NULL));
    ringBuffer ring(5);

    producer p(&lockGuard, &spaceAvailable, &ring, 10);
    producer q(&lockGuard, &spaceAvailable, &ring, 10);
    consumer c1(&lockGuard, &dataAvailable, &ring);
    consumer c2(&lockGuard, &dataAvailable, &ring);

    p.start();
    q.start();
    c1.start();
    c2.start();

    pthread_join(p.thread, NULL);
    pthread_join(q.thread, NULL);

    c1.stop();
    c2.stop();

    pthread_join(c1.thread, NULL);
    pthread_join(c2.thread, NULL);

    return 0;
}
