/*
	작성자 : 윤정도
	PCH 파일입니다.
*/


#pragma once

#pragma warning(push, 0)
    //26495: is uninitialized.Always initialize a member variable(type.6)
    //26812: is unscoped.Prefer 'enum class' over 'enum' (Enum.3)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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

using namespace JCore;

#define ON		1
#define OFF		0

//출력 여부
#define Print	OFF

// JCore::Container 테스트 수행 여부
#define ContainerTestEnabled        ON
// 컨테이너 개발 테스트 코드를 실행 여부
#define ContainerImplTestEnabled    OFF

// JCore::Primitives 테스트 수행 여부
#define PrimitivesTestEnabled       ON

// JCore::Sync 테스트 수행 여부
#define SyncTestEnabled             ON


#if ContainerTestEnabled == ON
    
    #define	TEST_ArrayStackTest				    ON
    #define	TEST_ArrayQueueTest				    ON
    #define	TEST_ArraysTest					    ON
    #define TEST_HashMapTest                    ON
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


#if PrimitivesTestEnabled == ON

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
#endif



#define TEST_AutoObjectTest				ON
#define TEST_ComparatorTest				ON
#define TEST_CoreTest					ON
#define TEST_EventTest                  ON
#define TEST_HasherTest					ON
#define TEST_MathTest					ON
#define TEST_MemoryTest					ON
#define TEST_RandomTest					ON
#define TEST_TimeTest					ON
#define TEST_TypeTraitsTest				ON

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


// 범위 메모리릭 체크
// @코드 획득 주소 : https://stackoverflow.com/questions/29174938/googletest-and-memory-leaks
class AutoMemoryLeakDetector
{
public:
    AutoMemoryLeakDetector() {
        _CrtMemCheckpoint(&memState_);
    }

    ~AutoMemoryLeakDetector() {
        _CrtMemState stateNow, stateDiff;
        _CrtMemCheckpoint(&stateNow);
        const int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);
        
        if (diffResult) {
            reportFailure(stateDiff.lSizes[1]);
            _CrtMemDumpStatistics(&stateDiff);
        }
    }
private:
    void reportFailure(unsigned int unfreedBytes) {
        FAIL() << "Memory leak of " << unfreedBytes << " byte(s) detected.";
    }
    _CrtMemState memState_;
};


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
inline Model::~Model() { PrintFormat("모델 %d 소멸\n", a); }
inline SuperModel::~SuperModel() { PrintFormat("슈퍼모델 %d 소멸\n", a); }

#pragma warning(pop)