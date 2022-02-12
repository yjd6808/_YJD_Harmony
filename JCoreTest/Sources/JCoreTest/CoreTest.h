/*
	작성자 : 윤정도
	PCH 파일입니다.
*/


#pragma once

#pragma warning(disable 26495) //  is uninitialized.Always initialize a member variable(type.6)
#pragma warning(disable 26812) //  is unscoped.Prefer 'enum class' over 'enum' (Enum.3)

#include "gtest/gtest.h"

#include <vector>
#include <iostream>
#include <string>
#include <typeinfo>
#include <algorithm>
#include <utility>
#include <string_view>
#include <crtdbg.h>
#include <unordered_map>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <stack>
#include <array>

#include <JCore/TypeTraits.h>

#define ON		1
#define OFF		0

#define Print	ON


#define ContainerTestEnabled        ON
#define ContainerImplTestEnabled    OFF

#if ContainerTestEnabled == ON

    #define	TEST_ArraysTest					    OFF
    #define	TEST_ArrayStackTest				    ON
    #define	TEST_ArrayQueueTest				    ON

    #if ContainerImplTestEnabled == ON
        #define	TEST_AVLTreeImplTest			OFF
        #define	TEST_AVLTreeMemoImplTest		OFF
        #define TEST_BinarySearchTreeImplTest	OFF
        #define TEST_DynamicHashMapImplTest		OFF
        #define TEST_SmartVectorImplTest		ON
        #define TEST_TwoThreeFourTreeImplTest	OFF
    #endif
#endif

#define TEST_AutoObjectTest				ON
#define TEST_ComparatorTest				ON
#define TEST_CoreTest					ON
#define TEST_HasherTest					OFF
#define TEST_MathTest					ON
#define TEST_MemoryTest					ON
#define TEST_PointerObserverTest		ON
#define TEST_RandomTest					OFF
#define TEST_SmartPointerTest			ON
#define TEST_StringTest					ON
#define TEST_StringUtilTest				ON
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
class MemoryLeakDetector
{
public:
    MemoryLeakDetector() {
        _CrtMemCheckpoint(&memState_);
    }

    ~MemoryLeakDetector() {
        _CrtMemState stateNow, stateDiff;
        _CrtMemCheckpoint(&stateNow);
        int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);
        
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