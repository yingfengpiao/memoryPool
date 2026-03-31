#include "../include/MemoryPool.h"
using namespace std;

namespace My_memoryPool
{
MemoryPool::MemoryPool(size_t SlotSize,size_t BlockSize)
    : SlotSize_(SlotSize)
    , BlockSize_(BlockSize)
    , FreeList_(nullptr)
    , CurSlot_(nullptr)
    , FirstBlock_(nullptr)
    , LastSlot_(nullptr)
    , MutexBlock_()
{}

MemoryPool::~MemoryPool(){
    Slot* temp=FirstBlock_;
    while(temp){
        Slot* next=temp->next;
        operator delete(reinterpret_cast<void*>(temp));
        temp=temp->next;
    }
}

void* MemoryPool::allocate(){
    Slot* node=popFreeList();
    if(node!=nullptr){ return node; }
    {
        lock_guard<mutex> lock(MutexBlock_);
        if(CurSlot_>=LastSlot_){
            if(allocateNewBlock()){
                return nullptr;
            }
        }
        node=CurSlot_;
        CurSlot_+=SlotSize_/sizeof(Slot*);
    }
    return node;
}

void MemoryPool::deallocate(void* ptr){
    if(ptr==nullptr) return;
    pushFreeList(reinterpret_cast<Slot*>(ptr));
}

bool MemoryPool::allocateNewBlock(){
    void* newBlock=operator new(BlockSize_);
    if(newBlock==nullptr){ return false; }
    reinterpret_cast<Slot*>(newBlock)->next=FirstBlock_;
    FirstBlock_=reinterpret_cast<Slot*>(newBlock);
    
    char* body=reinterpret_cast<char*>(newBlock)+sizeof(Slot*);
    size_t paddingSize=padPointer(body,SlotSize_);
    CurSlot_=reinterpret_cast<Slot*>(body+paddingSize);

    LastSlot_=reinterpret_cast<Slot*>(reinterpret_cast<size_t>(newBlock)+BlockSize_-SlotSize_+1);
    return true;
}

size_t MemoryPool::padPointer(char* p, size_t align){
    size_t temp = (reinterpret_cast<size_t>(p) % align);
    return temp == 0 ? 0 : (align - temp);
}

Slot* MemoryPool::popFreeList(){
    while(true){
        Slot* oldHead=FreeList_.load(memory_order_acquire);
        if(oldHead==nullptr) return nullptr;
        Slot* newHead=nullptr;
        try
        {
            newHead=oldHead->next.load(memory_order_relaxed);
        }
        catch(...)
        {
            continue;
        }
        if(FreeList_.compare_exchange_weak(oldHead,newHead,memory_order_release,memory_order_relaxed))
        {
            return oldHead;
        }
    }

}
bool MemoryPool::pushFreeList(Slot* node){
    while(true){
        Slot* oldHead=FreeList_.load(memory_order_relaxed);
        node->next.store(oldHead,memory_order_relaxed);
        if(FreeList_.compare_exchange_weak(oldHead,node,memory_order_release,memory_order_relaxed)){
            return true;
        }
    }
}

MemoryPool* HashBucket::memoryPools_[MEMORY_POOL_NUM]={nullptr};

MemoryPool& HashBucket::getMemoryPool(size_t size){
    if(size>MAX_SLOT_SIZE) throw std::invalid_argument("Size exceeds maximum slot size");
    size_t index=(size-1)/SLOT_BASE_SIZE;
    if(memoryPools_[index]==nullptr){
        memoryPools_[index]=new MemoryPool((index+1)*SLOT_BASE_SIZE);
    }
    return *memoryPools_[index];
}

}