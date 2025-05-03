#include <iostream>
#include "include/MemoryPool.h"
#include "include/StringQueue.h"
int main(){
    printf("\n====== FIRST FIT TESTING =====\n");
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


    printf("\n====== BEST FIT TESTING =====\n");
    BestFitPool pool2(100);
    pool2.cyclePool();
    //TEST LOOP, FIRST FIT
    void *b1, *b2, *b3, *b4, *b5, *b6;
    b1 = pool2.allocate(1);
    b2 = pool2.allocate(2);
    b3 = pool2.allocate(3);
    b4 = pool2.allocate(4);
    b5 = pool2.allocate(5);
    b6 = pool2.allocate(6);

    pool2.debugPrint();

    pool2.free(b1);
    pool2.free(b2);
    pool2.free(b3);

    pool2.free(b6);
    pool2.debugPrint();

    pool2.allocate(6);
    pool2.debugPrint();

    //Remember that it allocates memory for look-forward so that it can find the next block of data. This increases any size by 4 bytes; the data size is still X
    //But the actual size is X + 4
    printf("\n====== STRING TESTING =====\n");
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
