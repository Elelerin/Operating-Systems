#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <limits.h>
#pragma pack(1)

class MemoryPool{
public:
    MemoryPool(const uint32_t size);
    ~MemoryPool();
    virtual void * allocate(uint32_t nbytes) = 0;
    virtual void free(void * block) = 0;
    virtual void debugPrint() = 0;
    void* cyclePool();
protected:
    uint32_t poolSize;
    void* allocatedMemory;
};

class FirstFitPool : virtual public MemoryPool{
public:
    FirstFitPool(const uint32_t size) : MemoryPool(size){};
    ~FirstFitPool();
    void* allocate(uint32_t nbytes);
    void free(void* block);
    void debugPrint() ;
private:

};

class BestFitPool : virtual public MemoryPool{
public:
    BestFitPool(const uint32_t size) : MemoryPool(size){};
    ~BestFitPool() {};
    void* allocate(uint32_t nbytes);
    void free(void* block);
    void debugPrint();
private:

};

#endif // MEMORYPOOL_H
