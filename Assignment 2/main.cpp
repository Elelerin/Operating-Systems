#include <iostream>
#include "include/MemoryPool.h"
#include "include/StringQueue.h"
int main(){
    std::cout << "Hello World\n";
    FirstFitPool pool(100);
    pool.cyclePool();
    //TEST LOOP, FIRST FIT
    void* a1, *a2, *a3, *a4, *a5, *a6;
    a1 = pool.allocate(1);
    a2 = pool.allocate(2);
    a3 = pool.allocate(3);
    a4 = pool.allocate(4);
    a5 = pool.allocate(5);
    a6 = pool.allocate(6);
    pool.debugPrint();

    pool.free(a1);
    //Free / Allocate look forward for blank blocks. There's no 'defrag' command. But I can add one.
    pool.free(a5);
    pool.free(a4);
    pool.free(a3);

    pool.debugPrint();

    pool.allocate(6);
    pool.debugPrint();


    //Remember that it allocates memory for look-forward so that it can find the next block of data. This increases any size by 4 bytes; the data size is still X
    //But the actual size is X + 4
    FirstFitPool stringTesting(508);
    StringQueue bert(&stringTesting);
    bert.insert("Testing");
    bert.insert("Testing1");
    bert.insert("Testing2");
    stringTesting.debugPrint();
    printf("Front String: %s\n", bert.peek());
    bert.remove();
    bert.remove();
    stringTesting.debugPrint();
    printf("Front String: %s\n", bert.peek());
}
