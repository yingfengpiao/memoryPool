#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include "../include/MemoryPool.h"

using namespace std;
using namespace My_memoryPool;

struct Pod {
    int a;
    double b;
};

class Test {
public:
    int x;
    Test(int v=0): x(v) {}
    ~Test() {}
};

static void benchmarkPool(size_t n) {
    cout << "\n=== memory pool [newElement/deleteElement] n=" << n << " ===\n";
    auto t0 = chrono::high_resolution_clock::now();
    for(size_t i=0; i<n; ++i) {
        Test* p = newElement<Test>(int(i));
        p->x = int(i);
        deleteElement(p);
    }
    auto t1 = chrono::high_resolution_clock::now();
    cout << "pool cost: "
         << chrono::duration<double, milli>(t1 - t0).count()
         << " ms\n";
}

static void benchmarkRaw(size_t n) {
    cout << "\n=== raw [new/delete] n=" << n << " ===\n";
    auto t0 = chrono::high_resolution_clock::now();
    for(size_t i=0; i<n; ++i) {
        Test* p = new Test(int(i));
        p->x = int(i);
        delete p;
    }
    auto t1 = chrono::high_resolution_clock::now();
    cout << "raw cost: "
         << chrono::duration<double, milli>(t1 - t0).count()
         << " ms\n";
}

static void benchmarkUseFree(size_t n) {
    cout << "\n=== HashBucket::useMemory/freeMemory n=" << n << " ===\n";
    vector<void*> arr;
    arr.reserve(n);
    auto t0 = chrono::high_resolution_clock::now();
    for(size_t i=0; i<n; ++i) {
        void* p = HashBucket::useMemory(sizeof(Pod));
        arr.push_back(p);
    }
    auto t1 = chrono::high_resolution_clock::now();
    for(void* p : arr) {
        HashBucket::freeMemory(p, sizeof(Pod));
    }
    auto t2 = chrono::high_resolution_clock::now();
    cout << "alloc cost: "
         << chrono::duration<double, milli>(t1 - t0).count()
         << " ms, free cost: "
         << chrono::duration<double, milli>(t2 - t1).count()
         << " ms\n";
}

static void benchmarkDeleteElementFreeMemory(size_t n) {
    cout << "\n=== memory pool [newElement/deleteElement] 2-step n=" << n << " ===\n";
    vector<Test*> arr;
    arr.reserve(n);

    for(size_t i=0; i<n; ++i) {
        arr.push_back(newElement<Test>(int(i)));
    }

    auto t0 = chrono::high_resolution_clock::now();
    for(Test* p : arr) {
        deleteElement(p);
    }
    auto t1 = chrono::high_resolution_clock::now();

    cout << "deleteElement cost: "
         << chrono::duration<double, milli>(t1 - t0).count()
         << " ms\n";
}
class P1 
{
    int id_;
};

class P2 
{
    int id_[5];
};

class P3
{
    int id_[10];
};

class P4
{
    int id_[20];
};
void BenchmarkMemoryPool(size_t ntimes, size_t nworks, size_t rounds)
{
	std::vector<std::thread> vthread(nworks); // 线程池
	size_t total_costtime = 0;
	for (size_t k = 0; k < nworks; ++k) // 创建 nworks 个线程
	{
		vthread[k] = std::thread([&]() {
			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
                    P1* p1 = newElement<P1>(); // 内存池对外接口
                    deleteElement<P1>(p1);
                    P2* p2 = newElement<P2>();
                    deleteElement<P2>(p2);
                    P3* p3 = newElement<P3>();
                    deleteElement<P3>(p3);
                    P4* p4 = newElement<P4>();
                    deleteElement<P4>(p4);
				}
				size_t end1 = clock();

				total_costtime += end1 - begin1;
			}
		});
	}
	for (auto& t : vthread)
	{
		t.join();
	}
	printf("%lu个线程并发执行%lu轮次，每轮次newElement&deleteElement %lu次，总计花费：%lu ms\n", nworks, rounds, ntimes, total_costtime);
}

void BenchmarkNew(size_t ntimes, size_t nworks, size_t rounds)
{
	std::vector<std::thread> vthread(nworks);
	size_t total_costtime = 0;
	for (size_t k = 0; k < nworks; ++k)
	{
		vthread[k] = std::thread([&]() {
			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
                    P1* p1 = new P1;
                    delete p1;
                    P2* p2 = new P2;
                    delete p2;
                    P3* p3 = new P3;
                    delete p3;
                    P4* p4 = new P4;
                    delete p4;
				}
				size_t end1 = clock();
				
				total_costtime += end1 - begin1;
			}
		});
	}
	for (auto& t : vthread)
	{
		t.join();
	}
	printf("%lu个线程并发执行%lu轮次，每轮次malloc&free %lu次，总计花费：%lu ms\n", nworks, rounds, ntimes, total_costtime);
}

int main() {
    const size_t N = 5000000;

    // correctness sanity
    Test* testObj = newElement<Test>(123);
    cout << "object x = " << testObj->x << "\n";
    deleteElement(testObj);

    benchmarkPool(N);
    benchmarkRaw(N);
    benchmarkUseFree(N);
    benchmarkDeleteElementFreeMemory(N);

    BenchmarkMemoryPool(10000, 1, 10); // 1线程，10轮，每轮10000次
    BenchmarkNew(10000, 1, 10); // 测试 new delete
    
    cout << "\n=== done ===\n";
    return 0;
}