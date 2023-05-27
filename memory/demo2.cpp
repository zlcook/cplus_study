#include <cstdlib>
#include <iostream>

class Airplane {
public:
    Airplane() {
        //std::cout << "ctr.this:" << this << "\n";
    }
    ~Airplane() {
        //std::cout << "~dctr.this:" << this << "\n";
    }
    struct AirplaneRep {
        unsigned long miles;
        char type;
    };
    
    void set(unsigned long m, char t) {
        rep.miles = m;
        rep.type = t;
    }
    unsigned long getMiles() { return rep.miles; }
    char getType() { return rep.type; }

    // new Airplane()会调用该方法用来申请内存
    static inline void* operator new(size_t sz);
    // 申请的空间只有当程序退出时才真正释放
    // delete (Airplane*)ptr; 会调用该方法用来释放内存
    static inline void operator delete(void* buf, size_t sz);
private:
    union {
        AirplaneRep rep;
        Airplane* next;
    };
public:
    static int CHUNK_SIZE;
    static Airplane* headOfFreeLists;
};

int Airplane::CHUNK_SIZE = 512;
Airplane* Airplane::headOfFreeLists = nullptr;

// 实现内存池
void* Airplane::operator new(size_t sz) {
    if (sizeof(Airplane) != sz)
        return ::operator new(sz);
    size_t size = Airplane::CHUNK_SIZE * sz;
    Airplane* obj;
    if (Airplane::headOfFreeLists == nullptr) {
        std::cout << "alloc size:" << size << "\n";
        // global new
        Airplane* p = static_cast<Airplane*>(::operator new(size));
        //void* p = std::malloc(size);
        if (p == 0) {
            throw std::bad_alloc();
        }
        obj = static_cast<Airplane*>(p);
        Airplane::headOfFreeLists = obj;
        for (size_t i = 0; i < Airplane::CHUNK_SIZE - 1; ++i) {
            obj[i].next = &obj[i+1];
        }
        obj[Airplane::CHUNK_SIZE - 1].next = nullptr;
    }
    obj = Airplane::headOfFreeLists;
    Airplane::headOfFreeLists = Airplane::headOfFreeLists->next;
    return obj;
}

// 将释放的空间回收到内存池中
void Airplane::operator delete(void* buf, size_t sz) {
    if (sizeof(Airplane) != sz) {
        std::cout << "delete size: " << sz;
        ::operator delete(buf); // global delete
    } else {
        Airplane* obj = static_cast<Airplane*>(buf);
        obj->next = Airplane::headOfFreeLists;
        Airplane::headOfFreeLists = obj;
    }
}

int main(int argc, char**argv) {
    const size_t N = 100;
    Airplane* obj[N];
    std::cout << "sizeof(Airplane):" << sizeof(Airplane) << "\n";
    for (unsigned long i = 0; i < 10; ++i) {
        obj[i] = new Airplane();
        obj[i]->set(i, 'A');
    }
    for (int i = 0; i < 10; ++i) {
        // 地址间隔sizeof(Airplane)
        std::cout << obj[i] << ", " << obj[i]->getMiles() << std::endl;
    }
    for (int i = 0; i < 10; ++i) {
        delete obj[i];
    }
    
    // 10个元素复用了上面的内存空间
    for (unsigned long i = 10; i < 20; ++i) {
        obj[i] = new Airplane();
        obj[i]->set(i, 'B');
    }
    for (int i = 10; i < 20; ++i) {
        // 地址间和上面刚释放的地址一致
        std::cout << obj[i] << ", " << obj[i]->getMiles() << std::endl;
    }
}
