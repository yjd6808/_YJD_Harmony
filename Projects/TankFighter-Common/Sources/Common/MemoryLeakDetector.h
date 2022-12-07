#pragma once

#include <crtdbg.h>
#include <cstdio>

// disable all warnings
#pragma warning(push, 0)

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
    static void reportFailure(unsigned int unfreedBytes) {
        printf("메모리 릭 발생 : %d bytes!!!\n", unfreedBytes);
    }

    _CrtMemState memState_{};
};


class MemoryLeakDetector
{
public:
    void StartDetect() {
        _CrtMemCheckpoint(&memState_);
    }
    int StopDetect() const {
        _CrtMemState stateNow, stateDiff;
        _CrtMemCheckpoint(&stateNow);

        const int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);

        if (diffResult) {
            return stateDiff.lSizes[1];
        }

        return 0;
    }
private:

    _CrtMemState memState_{};
};

#pragma warning(pop)