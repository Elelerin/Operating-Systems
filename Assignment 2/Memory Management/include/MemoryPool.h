#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include <cstdint>

class MemoryPool{
public:
    MemoryPool(const uint32_t size);
    ~MemoryPool();
    virtual void * Allocate(const uint32_t nbytes);
    virtual void Free(void * block);
    virtual void DebugPrint();
private:
    uint32_t poolSize;
    void* allocatedMemory;
    void* page [128];
    vector<int32_t> blocks;
};

#endif // MEMORYPOOL_H
