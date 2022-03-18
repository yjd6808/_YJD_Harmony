#pragma once
#define _CRTDBG_MAP_ALLOC


#include <iostream>
#include <utility>
#include <type_traits>
#include <time.h>
#include <chrono>
#include <functional>
#include <mutex>
#include <memory>
#include <queue>
#include <stack>
#include <list>
#include <unordered_map>
#include <atomic>

#include <JCore/Core.h>
#include <JCore/StringUtil.h>
#include <JCore/String.h>
#include <JCore/TypeTraits.h>
#include <JCore/Type.h>
#include <JCore/Random.h>
#include <JCore/Deletor.h>
#include <JCore/StaticString.h>
#include <JCore/Time.h>
#include <JCore/SmartPointer.h>
#include <JCore/Exception.h>
#include <JCore/PointerObserver.h>
#include <JCore/AutoObject.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Container/ArrayStack.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/Vector.h>
#include <JCore/Container/ListStack.h>
#include <JCore/Container/ListQueue.h>
#include <JCore/Container/LinkedList.h>
#include <JCore/Mutex.h>
#include <JCore/Mutex.h>

using namespace JCore;
using namespace JCore;
using namespace std;


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
        printf("메모리 릭 발생 : %d bytes!!!\n", unfreedBytes);
    }
    _CrtMemState memState_;
};