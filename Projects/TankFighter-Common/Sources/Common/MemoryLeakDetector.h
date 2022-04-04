#pragma once

#include <crtdbg.h>
#include <cstdio>

// ���� �޸𸮸� üũ
// @�ڵ� ȹ�� �ּ� : https://stackoverflow.com/questions/29174938/googletest-and-memory-leaks
class MemoryLeakDetector
{
public:
    MemoryLeakDetector() {
        _CrtMemCheckpoint(&memState_);
    }

    ~MemoryLeakDetector() {
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
        printf("�޸� �� �߻� : %d bytes!!!\n", unfreedBytes);
    }
    _CrtMemState memState_;
};

