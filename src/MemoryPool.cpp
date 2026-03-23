#include "../include/MemoryPool.h"
using namespace std;

namespace My_memoryPool
{
MemoryPool::MemoryPool(size_t SlotSize_,size_t BlockSize)
    : SlotSize_(SlotSize)
    , BlockSize_(BlockSize)
    , FreeList_(nullptr)
    , CurSlot_(nullptr)
    , FirstBlock_(nullptr)
    , LastSlot_(nullptr)
    , MutexBlock_()
{}

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
        Slot* oldhead=FreeList_.load(memoryPo)
    }

}
bool MemoryPool::pushFreeList(Slot* slot){
{

}

}