#include<iostream>
#include<cmath>
#include<string>
using namespace std;

namespace My_memoryPool
{
#define MEMORY_POOL_NUM 64
#define SLOT_BASE_SIZE 8
#define MAX_SLOT_SIZE 512

struct Slot 
{
    atomic<Slot*> next;
};



}
