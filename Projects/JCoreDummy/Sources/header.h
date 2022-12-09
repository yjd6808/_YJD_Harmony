#pragma once
#define _CRTDBG_MAP_ALLOC


#include <JCore/Random.h>
#include <JCore/TypeTraits.h>
#include <JCore/Time.h>
#include <JCore/Limit.h>

#include <JCore/Container/HashMap.h>
#include <JCore/Container/Vector.h>

#include <JCore/Primitives/Atomic.h>
#include <JCore/Primitives/SmartPtr.h>
#include <JCore/Primitives/String.h>

#include <JCore/Sync/WaitHandle.h>

#include <JCore/Encoding/CodePage.h>


#include <JCore/Wrapper/WinApi.h>
#include <JCore/Wrapper/CRuntime.h>

#include <JCore/Utils/Console.h>
#include <JCore/Threading/Thread.h>

using namespace JCore;
using namespace std;


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
        printf("메모리 릭 발생 : %d bytes!!!\n", unfreedBytes);
    }
    _CrtMemState memState_;
};