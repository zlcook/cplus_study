#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "MyAllocator.h"

namespace study {

class Foo {
public:
    Foo() {
        std::cout << "ctr.this:" << this << "\n";
    }
    ~Foo() {
        std::cout << "dctr.this:" << this << "\n";
    }

    int get() { return a;}

    DECARE_POOL_ALLOC()

private:
    int a;
};

IMPLEMENT_POOL_ALLOC(Foo)

} // end namespace


using namespace study;
int main(int argc, char** argv) {
    /**
     *  new(size_t sz) size:4
        allocate size: 20
        ctr.this:0x15ae06a60
        dctr.this:0x15ae06a60
        delete(void* buf, size_t sz) size:4
        new[](size_t sz) size:28
        ctr.this:0x15ae06a90
        ctr.this:0x15ae06a94
        ctr.this:0x15ae06a98
        dctr.this:0x15ae06a98
        dctr.this:0x15ae06a94
        dctr.this:0x15ae06a90
        delete[](void* buf, size_t sz) size:28
     * 
     */
    Foo* f = new Foo();
    delete f;

    Foo* fa = new Foo[3];
    delete[] fa;

    std::cout << "\n======\n";
    Foo* array[100];
    for (int i = 0; i < 10; ++i) {
        array[i] = new Foo();
    }
    for (int i = 0; i < 10; ++i) {
        std::cout << array[i] << std::endl;
    }

    for (int i = 0; i < 10; ++i) {
        delete array[i];
    }

    for (int i = 10; i < 20; ++i) {
        array[i] = new Foo();
    }
}
