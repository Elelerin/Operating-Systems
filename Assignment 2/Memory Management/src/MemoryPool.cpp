#include "../include/MemoryPool.h"

using namespace MemoryPool{
    MemoryPool(const uint32_t size){
        size = poolSize;
        void* allocatedMemory = malloc(size);
        void* page [128];
    }

    virtual void Free(void * block){

    };
}

class FirstFitPool : virtual public MemoryPool{
public:
    FirstFitPool(const uint32_t size) : MemoryPool(size){}
private:

};

class BestFitPool : virtual public MemoryPool{
public:
    BestFitPool(const uint32_t size) : MemoryPool(size){}
private:

};
