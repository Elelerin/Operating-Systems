#include "../include/MemoryPool.h"
#define SECTOR_END 45
struct dataVariable{
    int32_t size;
    void* data;
};

MemoryPool::MemoryPool(const uint32_t size){
    poolSize = size;
    this->allocatedMemory = malloc(size + 8);
    *(int32_t*)allocatedMemory = -size;
    *(int32_t*)(allocatedMemory + size + 4) = SECTOR_END;
}

MemoryPool::~MemoryPool(){

}

void MemoryPool::free(void* block){

};

#define __toInteger(currentLook) *(int32_t*)currentLook
#define __freeSpace(currentLook) abs(*(int32_t*)currentLook)
void* FirstFitPool::allocate(const uint32_t nBytes){
    void* currentLook = allocatedMemory;
    uint32_t freeMem = 0;

    while(__toInteger(currentLook) != SECTOR_END) {
        if(__toInteger(currentLook) < 0) { //Free block
            freeMem = __freeSpace(currentLook);
            //Try to merge with next free blocks
            void* tempLook = (void*)((char*)currentLook + freeMem + 4);
            while(__toInteger(tempLook) < 0 && __toInteger(tempLook) != SECTOR_END) {
                freeMem += __freeSpace(tempLook) + 4;
                __toInteger(currentLook) = -freeMem;
                tempLook = (void*)((char*)tempLook + __freeSpace(tempLook) + 4 + 1);
            }
            //Check if this block is large enough
            if(nBytes + 4 <= freeMem) {
                // Allocate here
                int32_t freeSectorSize = freeMem - 4 - nBytes;
                __toInteger(currentLook) = nBytes; //Mark allocated
                void* userData = (void*)((char*)currentLook + 4);
                if(freeSectorSize >= 4) {
                    void* nextBlock = (void*)((char*)currentLook + 4 + nBytes);
                    __toInteger(nextBlock) = -freeSectorSize; //New free block
                }
                return userData;
            }
        }
        //Only hits here if not enough space, move to next block.
        currentLook = (void*)((char*)currentLook + __freeSpace(currentLook) + 4);
    }
    printf("Error. Out of allocateable space\n");
    return nullptr;
}

void FirstFitPool::free(void* block){
    //Set the void to negative. This marks it as free w/ enough space.
    //We have to shift back because of the integer at the starting spot.
    void* align = block - 4;
    printf("INTEGER: %d\n", __toInteger(align));
    __toInteger(block) = -(__toInteger(align) + 4);
}

//Probably will be the same for both of these
void FirstFitPool::debugPrint() {
    void* currentLook = this->allocatedMemory;
    uint32_t currBlock = 0;
    int shift = 0;

    while(__toInteger(currentLook) != SECTOR_END){
        shift += __freeSpace(currentLook) + 4;
        printf("CURRENT BLOCK %4d, SIZE: %d, ALLOCATED:%s, shift:%d\n",
               currBlock,
               __freeSpace(currentLook),
               __toInteger(currentLook) < 0 ? "FLSE" : "TRUE",
               shift);


        currentLook = (void*)((char*)currentLook + __freeSpace(currentLook) + 4);
        currBlock++;
    }

    #define DEBUG 0
    #if DEBUG == 1
    currentLook = this->allocatedMemory;
    for(int i = 0;i < 100; i++){
        printf("%d: %d\n", i, __toInteger(currentLook));
        currentLook ++;
    }
    #endif
};

FirstFitPool::~FirstFitPool() {}

class BestFitPool : virtual public MemoryPool{
public:
    BestFitPool(const uint32_t size) : MemoryPool(size){}
private:

};
