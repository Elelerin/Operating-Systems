#ifndef STRINGQUEUE_H
#define STRINGQUEUE_H
#include "../include/MemoryPool.h"
#include <cstring>

class StringQueue
{
    public:
        StringQueue(FirstFitPool* _pool);
        virtual ~StringQueue();
        void insert(const char * s);
        char* peek();
        void remove();

    protected:

    private:
        FirstFitPool* memory;
};

#endif // STRINGQUEUE_H
