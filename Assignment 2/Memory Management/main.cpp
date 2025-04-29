#include <iostream>
#include "include/MemoryPool.h"

int main(){
    std::cout << "Hello World\n";
    FirstFitPool pool(100);
    pool.allocate(4);
    pool.allocate(4);
    pool.allocate(4);
    pool.debugPrint();
}
