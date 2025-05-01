#include "../include/StringQueue.h"

StringQueue::StringQueue(FirstFitPool* _pool){
    memory = _pool;
}

StringQueue::~StringQueue(){
    memory = nullptr;
}

void StringQueue::insert(const char * s){
    //Successful write if true
    void* dataBytes = this->memory->allocate(strlen(s) + 1);
    if(dataBytes == nullptr){
        return;
    }

    std::memcpy((char*)dataBytes, s, strlen(s));
    printf("STRING ALLOCATED: %s\n", dataBytes);

}

char* StringQueue::peek(){
    //Grabs the data.
    return (char*)(this->memory->cyclePool() + 4);
}

void StringQueue::remove(){
    printf("Pool: Removed String %s\n", this->peek());
    this->memory->free((void*)peek());
}


