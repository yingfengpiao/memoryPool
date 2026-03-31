#include<iostream>
#include"../include/MemoryPool.h"
using namespace std;
using namespace My_memoryPool;
class Test
{
private:
    int a_;
public:
    Test(int a):a_(a)
    { cout<<"Test constructor: "<<a_<<endl; }
    ~Test(){ cout<<"Test destructor: "<<a_<<endl; }
};

int main()
{
    Test* t1=newElement<Test>(666);
    
    return 0;
}  