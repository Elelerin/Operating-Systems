#include "../include/MemoryPool.h"
#define SECTOR_END 0xFFFFFF
#include <iostream>

#define __toInteger(currentLook) *(int32_t*)currentLook
#define __freeSpace(currentLook) abs(*(int32_t*)currentLook)

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

void* MemoryPool::cyclePool(){
    void* currentLook = this->allocatedMemory;
    while(__toInteger(currentLook) <= 0){
        currentLook += __freeSpace(currentLook) + 4;
        if(__toInteger(currentLook) == SECTOR_END){
            printf("CYCLE POOL ERROR: DATAPOOL IS EMPTY\n");
            return nullptr;
        }
    }
    return currentLook;
}


void* FirstFitPool::allocate(uint32_t nBytes){
    void* currentLook = allocatedMemory;
    uint32_t freeMem = 0;

    if(nBytes == 0){
        printf("ERROR: CANNOT ALLOCATE 0 MEMORY");
        return nullptr;
    }

    while(__toInteger(currentLook) != SECTOR_END) {
        if(__toInteger(currentLook) < 0) { //Free block
            freeMem = __freeSpace(currentLook);
            //Try to merge with next free blocks
            void* tempLook = (void*)((char*)currentLook + freeMem + 4);
            while(__toInteger(tempLook) < 0 && __toInteger(tempLook) != SECTOR_END) {
                __toInteger(currentLook) -= (__freeSpace(tempLook) + 4);
                tempLook = (void*)((char*)tempLook + __freeSpace(tempLook) + 4 + 1);
            }
            //Check if this block is large enough
            if(nBytes + 4 <= freeMem) {
                //Allocate here
                int32_t freeSectorSize = freeMem - 4 - nBytes;
                __toInteger(currentLook) = nBytes; //Mark allocated
                void* userData = (void*)((char*)currentLook + 4);
                void* nextBlock = (void*)((char*)currentLook + 4 + nBytes);
                if(freeSectorSize < 4) {
                    __toInteger(currentLook) += freeSectorSize + 4
                    ;

                }else{
                    __toInteger(nextBlock) = -freeSectorSize;
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
    int32_t freeMem = __freeSpace(align);
    void* tempLook = (void*)((char*)align + freeMem + 4);

    __toInteger(align) = -(__toInteger(align));
    while(__toInteger(tempLook) < 0 && __toInteger(tempLook) != SECTOR_END) {
        __toInteger(align) -= (__freeSpace(tempLook) + 4);
        tempLook = (void*)((char*)tempLook + __freeSpace(tempLook) + 4 + 1);
    }
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
    printf("\n");
    #define DEBUG 0
    #if DEBUG == 1
    //Playing blind here. Using this to figure out where the formulas I'm writing are aligning cause I had issues.
    currentLook = this->allocatedMemory;
    for(int i = 0;i < 100; i++){
        printf("%d: %d\n", i, __toInteger(currentLook));
        currentLook ++;
    }
    #endif
};


FirstFitPool::~FirstFitPool() {}

void* BestFitPool::allocate(uint32_t nBytes){
    void* currentLook = this->allocatedMemory;
    uint32_t freeMem = 0;
    uint32_t bestSize = INT_MAX;
    void* bestLocation = nullptr;

    if(nBytes == 0){
        printf("ERROR: CANNOT ALLOCATE 0 MEMORY");
        return nullptr;
    }

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
                if(bestSize > freeMem + 4){
                    bestLocation = currentLook;
                    bestSize = freeMem;
                }
            }
        }


        //Only hits here if not enough space, move to next block.
        currentLook = (void*)((char*)currentLook + __freeSpace(currentLook) + 4);
    }

    if(bestLocation != nullptr){
        //Allocate here
        int32_t freeSectorSize = freeMem - 4 - nBytes;
        __toInteger(bestLocation) = nBytes;
        void* userData = (void*)((char*)bestLocation + 4);
        void* nextBlock = (void*)((char*)bestLocation + 4 + nBytes);
        if(freeSectorSize >= 4) {
            __toInteger(nextBlock) = -freeSectorSize; //New free block
        }else{
            std::cout << __toInteger(bestLocation) << std::endl;
            __toInteger(bestLocation) += freeSectorSize;
            std::cout << __toInteger(bestLocation) << std::endl;
        }
        return userData;
    }

    printf("Error. Out of allocateable space OR disk too fragmented to find free location.\nPlease run scan / de-fragmentation operation if implemented.\n");
    return nullptr;
}

void BestFitPool::free(void* block){
    //Set the void to negative. This marks it as free w/ enough space.
    //We have to shift back because of the integer at the starting spot.
    void* align = block - 4;
    printf("INTEGER: %d\n", __toInteger(align));
    __toInteger(align) = -(__toInteger(align));
}

void BestFitPool::debugPrint() {
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
    //Playing blind here. Using this to figure out where the formulas I'm writing are aligning cause I had issues.
    currentLook = this->allocatedMemory;
    for(int i = 0;i < 100; i++){
        printf("%d: %d\n", i, __toInteger(currentLook));
        currentLook ++;
    }
    #endif
};



