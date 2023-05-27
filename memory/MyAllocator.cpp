#include <cstdlib>
#include <iostream>

/**
 * @brief 
 * 如果在每个类中都重写operator new/delete还是很麻烦的，
 * 这部分逻辑可以提取出来复用。
 * 每个对象MyAllocator都是一个分配器，内部维护一个freeList.
 * 不同的allocator object内有不同的freeList
 * 
 */
class MyAllocator {
private:
    /**
     * |----------------------
     * |next       | next    |
     * |-|---------^----------
     *   |_________|
     */
    struct Node {
        Node* next;
    };

public:
    void* allocate(size_t sz);
    void dealocate(void* buf, size_t sz);

private:
    Node* freeStore = nullptr;
    const int kCHUNK_SIZE = 5;
};

/**
 * @brief 
 * 
 * @param sz : 单个对象大小
 * @return void* 返回分配的地址
 */
void* MyAllocator::allocate(size_t sz) {
    Node* p;
    if (!freeStore) {
        if (sizeof(Node*) > sz) {
            std::cout << "sizeof(Node*) > sz "
                      << sizeof(Node*) << "," << sz << std::endl;
            sz = sizeof(Node*);
        }
        size_t chunk = kCHUNK_SIZE * sz;
        std::cout << "allocate size: " << chunk << std::endl;
        // freeStore = (obj*)malloc(chunk);
        p = freeStore = (Node*)::operator new(chunk);
        // 0用于返回
        for (int i = 0; i < kCHUNK_SIZE - 1; ++i) {
            p->next = (Node*)((char*)p + sz);
            p = p->next;
        }
        p->next = nullptr;
    }
    p = freeStore;
    freeStore = freeStore->next;
    return p;
}

// 回收内存到到freeList
void MyAllocator::dealocate(void* obj, size_t sz) {
    (static_cast<Node*>(obj))->next = freeStore;
    freeStore = (Node*)obj;
}


class Foo {
public:
    Foo() {
        std::cout << "ctr.this:" << this << "\n";
    }
    ~Foo() {
        std::cout << "dctr.this:" << this << "\n";
    }

private:
    int a;

public:
    void* operator new(size_t sz) {
        std::cout << "new(size_t sz) size:" << sz << std::endl;
        return allocator.allocate(sz);
    }
    void operator delete(void* buf, size_t sz) {
        std::cout << "delete(void* buf, size_t sz) size:" << sz << std::endl;
        return allocator.dealocate(buf, sz);
    }

    void* operator new[](size_t sz) {
        std::cout << "new[](size_t sz) size:" << sz << std::endl;
        return ::operator new[](sz);
    }
    void operator delete[](void* buf, size_t sz) {
        std::cout << "delete[](void* buf, size_t sz) size:" << sz << std::endl;
        return ::operator delete[](buf, sz);
    }
    
private:
    static MyAllocator allocator;
};

MyAllocator Foo::allocator;

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
