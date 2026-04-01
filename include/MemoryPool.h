#include<iostream>
#include<cmath>
#include<string>
#include<atomic>
#include<mutex>
using namespace std;

namespace My_memoryPool
{
const size_t MEMORY_POOL_NUM = 64;
const size_t SLOT_BASE_SIZE = 8;
const size_t MAX_SLOT_SIZE = 512;

struct Slot 
{
    atomic<Slot*> next;
};

class MemoryPool{
private:
    size_t SlotSize_;
    size_t BlockSize_;
    atomic<Slot* > FreeList_;
    Slot* CurSlot_;
    Slot* FirstBlock_;
    Slot* LastSlot_;
    mutex MutexBlock_;

public:
    MemoryPool(size_t SlotSize,size_t BlockSize=4096);
    ~MemoryPool();
    void* allocate();
    void deallocate(void* ptr);

private:
    Slot* popFreeList();
    bool pushFreeList(Slot* node);
    bool allocateNewBlock();
    size_t padPointer(char* p,size_t align);
};

class HashBucket{
private:
    static MemoryPool* memoryPools_[MEMORY_POOL_NUM];
    static MemoryPool& getMemoryPool(size_t size);

public:
    static void freeMemory(void* ptr,size_t size){
        if(!ptr) return;
        if(size>MAX_SLOT_SIZE) { operator delete(ptr); return; }
        size_t index=(size+7)/SLOT_BASE_SIZE-1;
        getMemoryPool((index+1)*SLOT_BASE_SIZE).deallocate(ptr);
    }
    static void* useMemory(size_t size){
        if(size<=0) return nullptr;
        if(size>MAX_SLOT_SIZE) return operator new(size);
        size_t index=(size+7)/SLOT_BASE_SIZE-1;
        return getMemoryPool((index+1)*SLOT_BASE_SIZE).allocate();
    }
    template<typename T, typename...Args>
    friend T*  newElement(Args&&... args);

    template<typename T>
    friend void deleteElement(T* p);
};

template<typename T, typename... Args>
T* newElement(Args&&... args)
{
    T* p = nullptr;
    // 根据元素大小选取合适的内存池分配内存
    if ((p = reinterpret_cast<T*>(HashBucket::useMemory(sizeof(T)))) != nullptr)
        // 在分配的内存上构造对象
        new(p) T(std::forward<Args>(args)...);
    return p;
}

template<typename T>
void deleteElement(T* p)
{
    // 对象析构
    if (p)
    {
        p->~T();
         // 内存回收
        HashBucket::freeMemory(reinterpret_cast<void*>(p), sizeof(T));
    }
}

}
