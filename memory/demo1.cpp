#include <cstdlib>
#include <iostream>
#include <new>

class Demo {
public:
    Demo() { std::cout << "ctr.this:" << this << ", a:" << a << "\n"; }
    Demo(int v): a(v) { std::cout << "ctr.this:" << this << ", a:" << a << "\n"; }
    ~Demo() {
        std::cout << "dctr.this:" << this << ", a:" << a << "\n";
        a = -1;
    }

    int getA() {return a;}
private:
    int a;
};
// no inline, required by [replacement.functions]/3
void* operator new(std::size_t sz)
{
    std::printf("1) new(size_t), size = %zu\n", sz);
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success
 
    if (void *ptr = std::malloc(sz))
        return ptr;
 
    throw std::bad_alloc(); // required by [new.delete.single]/3
}
 
// no inline, required by [replacement.functions]/3
void* operator new[](std::size_t sz)
{
    std::printf("2) new[](size_t), size = %zu\n", sz);
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success
 
    if (void *ptr = std::malloc(sz))
        return ptr;
 
    throw std::bad_alloc(); // required by [new.delete.single]/3
}

void operator delete(void* ptr)
{
    std::puts("3) delete(void*)");
    std::free(ptr);
}
 
void operator delete(void* ptr, std::size_t size)
{
    std::printf("4) delete(void*, size_t), size = %zu\n", size);
    std::free(ptr);
}
 
void operator delete[](void* ptr)
{
    std::puts("5) delete[](void* ptr)");
    std::free(ptr);
}
 
void operator delete[](void* ptr, std::size_t size)
{
    std::printf("6) delete[](void*, size_t), size = %zu\n", size);
    std::free(ptr);
}

int main(int argc, char** argv) {
    Demo* d = new Demo();
    delete d;

    Demo* array = new Demo[4];
    delete[] array;

    char* p = new char[sizeof(Demo)*3];
    Demo* p1 = new(p)Demo();
    Demo* p2 = new(p+sizeof(Demo))Demo();
    delete[] p;

    // equal to Demo* p = new Demo();  delete p;
    Demo* demo = (Demo*)std::malloc(sizeof(Demo));
    if (demo != nullptr) {
        new(demo)Demo(5); // call
    } else {
        throw std::bad_alloc();
    }
    demo->~Demo();
    free(demo);

    // equal to Demo* p = new Demo();  delete p;
    void* mem = operator new(sizeof(Demo)); // 调用了malloc
    Demo* ptr = static_cast<Demo*>(mem);
    //ptr->Demo::Demo();// 只有编译器可以这么直接调用ctr
    new(ptr)Demo(2);
    std::cout << "a=" << ptr->getA() << "\n";
    ptr->~Demo();
    std::cout << "a=" << ptr->getA() << "\n";
    operator delete(ptr, sizeof(Demo));// 调用了 free(ptr)
}
/**
 * 1) new(size_t), size = 4
 * ctr
 * dctr
 * 3) delete(void*)
 * 2) new[](size_t), size = 32
 * ctr
 * ctr
 * ctr
 * ctr
 * dctr
 * dctr
 * dctr
 * dctr
 * 5) delete[](void* ptr)
 */