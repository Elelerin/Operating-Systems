#include <iostream>
#include "include/MemoryPool.h"

int main(){
    std::cout << "Hello World\n";
    BestFitPool pool(100);
    void* a1 = pool.allocate(8);
    pool.allocate(16);
    void* a2 = pool.allocate(4);

    pool.free(a1);
    pool.free(a2);
    pool.allocate(4);
    pool.debugPrint();
}
