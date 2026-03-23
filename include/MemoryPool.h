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

class HashBucket{
private:


public:



    template<typename T, typename...Args>
    friend T*  newElement(Args&&... args);

    template<typename T>
    friend void deleteElement(T* p);
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
    void* construct(void* ptr,size_t size);
    void destroy(void* ptr,size_t size);
    

private:
    Slot* popFreeList();
    bool pushFreeList();
    bool allocateNewBlock();
    size_t padPointer(char* p,size_t align);
    
};

}
