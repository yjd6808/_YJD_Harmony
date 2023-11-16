/*
	작성자 : 윤정도
	PCH 파일입니다.
*/


#pragma once

#pragma warning(push, 0)
    //26495: is uninitialized.Always initialize a member variable(type.6)
    //26812: is unscoped.Prefer 'enum class' over 'enum' (Enum.3)


#include <JCore/Core.h>

#include "gtest/gtest.h"

#include <vector>
#include <iostream>
#include <string>
#include <typeinfo>
#include <algorithm>
#include <utility>
#include <string_view>
#include <crtdbg.h>
#include <thread>
#include <unordered_map>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <stack>
#include <array>

#include <JCore/TypeTraits.h>
#include <JCore/Limit.h>
#include <JCore/Allocator/DefaultArrayAllocator.h>
#include <JCore/Debug/MemoryLeakDetector.h>
#include <JCore/Debug/MemoryPoolLeakDetector.h>

USING_NS_JC;
USING_NS_STD;

#define ON		1
#define OFF		0

//출력 여부
#define Print	OFF

#define TestEnabled                 OFF     // 전체 테스트 수행 여부
#define BaseTestEnabled             ON      // JCore 테스트 수행 여부
#define ContainerTestEnabled        ON      // JCore::Container 테스트 수행 여부
#define ContainerImplTestEnabled    OFF     // 컨테이너 개발 테스트 코드를 수행 여부
#define FileSystemTestEnabled       ON      // JCore::FileSystem 테스트 코드 수행 여부
#define PoolTestEnabled             ON      // JCore::Pool 테스트 코드 수행 여부
#define PrimitivesTestEnabled       ON      // JCore::Primitives 테스트 수행 여부
#define RAIITestEnabled             ON      // JCore::RAII 테스트 수행 여부
#define SyncTestEnabled             ON      // JCore::Sync 테스트 수행 여부
#define ThreadingTest               ON      // JCore::Threading 테스트 수행 여부
#define WrapperTest                 ON      // JCore::Wrapper 테스트 수행 여부

// 개별 테스트 수행시 사용
#if TestEnabled == OFF
	#define TEST_AccessibleObjectPoolTest           ON
	#define TEST_ArraysTest                         ON
#endif

#if TestEnabled == ON

    #if ContainerTestEnabled == ON
        
        #define	TEST_ArrayStackTest				    ON
        #define	TEST_ArrayQueueTest				    ON
        #define	TEST_ArraysTest					    ON
        #define TEST_HashMapTest                    ON
		#define TEST_HashSetTest                    ON
		#define TEST_TreeMapTest                    ON
		#define TEST_TreeSetTest                    ON
        #define	TEST_LinkedListTest					ON
        #define	TEST_ListQueueTest					ON
        #define	TEST_ListStackTest					ON
        #define	TEST_VectorTest					    ON
        
        #if ContainerImplTestEnabled == ON

            #define	TEST_AVLTreeImplTest			OFF
            #define	TEST_AVLTreeMemoImplTest		OFF
            #define TEST_BinarySearchTreeImplTest	OFF
            #define TEST_DynamicHashMapImplTest		OFF
            #define TEST_SmartVectorImplTest		ON
            #define TEST_TwoThreeFourTreeImplTest	OFF

        #endif
    #endif

	#if RAIITestEnabled == ON
		#define TEST_AutoPtrTest                ON
		#define TEST_AutoRefTest                ON
	#endif

	#if FileSystemTestEnabled == ON 
		#define TEST_DirectoryTest              ON
		#define TEST_FileUtilTest               ON
		#define TEST_FileStreamTest             ON
		#define TEST_PathTest                   ON
	#endif

	#if PoolTestEnabled == ON
		#define TEST_MemoryPoolTest             ON
		#define TEST_AccessibleObjectPoolTest   ON
	#endif

    #if PrimitivesTestEnabled == ON

        #define TEST_AtomicTest                 ON
		#define TEST_BitFlagTest                ON
        #define TEST_ObserverPtrTest		    ON
        #define TEST_SmartPtrTest			    ON
        #define TEST_StaticStringTest           ON
        #define TEST_StringTest					ON
        #define TEST_StringUtilTest				ON

    #endif

    #if SyncTestEnabled == ON

        #define TEST_EventLockTest              ON
        #define TEST_LockGuardTest              ON
        #define TEST_NormalLockTest             ON
        #define TEST_NormalRwLockTest           ON
        #define TEST_SemaphoreTest              ON
        #define TEST_SpinLockTest               ON
        #define TEST_RecursiveLockTest          ON
        #define TEST_WaitHandleTest             ON
    #endif

	#if ThreadingTest == ON
		#define TEST_ThreadTest                 ON
		#define TEST_ThreadLocalTest            ON
		#define TEST_ThreadPoolTest             ON
		#define TEST_SchedulerTest              ON
	#endif

	#if WrapperTest == ON
		#define TEST_CRuntimeTest ON
	#endif

    #if BaseTestEnabled == ON

        #define TEST_AutoObjectTest				ON
        #define TEST_ComparatorTest				ON
        #define TEST_CoreTest					ON
        #define TEST_EventTest                  ON
        #define TEST_ExceptionTest              ON
        #define TEST_HasherTest					ON
        #define TEST_MathTest					ON
        #define TEST_MemoryTest					ON
        #define TEST_RandomTest					ON
        #define TEST_TimeTest					ON
        #define TEST_TypeTraitsTest				ON

    #endif



#endif 
template <typename... Args>
void PrintFormat(const char* fmt, Args&&... args) {
#if Print == ON
	printf(fmt, JCore::Forward<Args>(args)...);
#endif
}

template <typename... Args>
void PrintFormat(Args&&... args) {
	if (sizeof...(args) == 0) {
	#if Print == ON
		printf("\n");
	#endif
	}
}



// https://stackoverflow.com/questions/1082192/how-to-generate-random-variable-names-in-c-using-macros
#define LeakCheckConcat(a, b) LeakCheckConcatInner(a, b)
#define LeakCheckConcatInner(a, b) a##b {[](Int32U unfreedBytes) { FAIL() << unfreedBytes << " 바이트 메모리릭\n"; }}
#define LeakCheck AutoMemoryLeakDetector LeakCheckConcat(LeakCheck, __COUNTER__)

#define MemoryPoolLeakCheckConcat(a, b) MemoryPoolLeakCheckConcatInner(a, b)
#define MemoryPoolLeakCheckConcatInner(a, b) a##b {&JCore::ArrayAllocatorPool_v, [](Int64U unDeallocatedBytes, int* detail) { \
    if (unDeallocatedBytes > 0) \
		FAIL(); \
}};
#define MemoryPoolLeakCheck AutMemoryPoolLeakDetector MemoryPoolLeakCheckConcat(MemoryPoolLeakCheck, __COUNTER__)

// 테스트용 오브젝트들
struct Animal
{
    virtual int GetAge() const = 0;

    Animal();
    virtual ~Animal();
};

struct Dog : Animal
{
    int GetAge() const override { return Age; };

    ~Dog() override;

    int Age = 300;
};

struct Bird : Animal
{
    int GetAge() const override { return Age; };

    ~Bird() override;

    int Dummy = 0;
    int Age = 400;
};

struct Model
{
    Model();
    Model(int _);
    Model(int _1, int _2);
    virtual ~Model();

    int a = 3;
    int b = 3;
};

struct SuperModel : Model
{
    SuperModel(int a) : Model(a) {}
    SuperModel() : Model(0) {}
    ~SuperModel() override;
};


template <typename T>
struct Template
{
    Template() { }
    Template(T _) { a = _; }
    virtual ~Template() {}

    T a = 3;
    T b = 3;
};

template <typename T>
struct ChildTemplate : Template<T>
{
    ChildTemplate() {}
    ChildTemplate(T o) : Template<T>(o) {}
    ~ChildTemplate() override {}
};



inline Animal::Animal() {
    PrintFormat("애니멀 생성\n");
}

inline Animal::~Animal() {
    PrintFormat("애니멀 소멸\n");
}

inline Dog::~Dog() {
    PrintFormat("개 소멸\n");
}

inline Bird::~Bird() {
    PrintFormat("날개 소멸\n");
}

inline Model::Model() { PrintFormat("모델 디폴트 생성\n"); }
inline Model::Model(int _) : a(_) { PrintFormat("모델 %d 생성\n", a); }
inline Model::Model(int _1, int _2) : a(_1), b(_2) { PrintFormat("모델 %d %d 생성\n", a, b); }
inline Model::~Model() { PrintFormat("모델 %d 소멸\n", a); }
inline SuperModel::~SuperModel() { PrintFormat("슈퍼모델 %d 소멸\n", a); }

int R(int min, int max);

#pragma warning(pop)