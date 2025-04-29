#include <iostream>
#include "include/MemoryPool.h"

int main(){
    std::cout << "Hello World\n";
    FirstFitPool pool(100);
    pool.allocate(4);
    void* n = pool.allocate(16);
    pool.allocate(4);
    pool.free(n);
    pool.debugPrint();
}
