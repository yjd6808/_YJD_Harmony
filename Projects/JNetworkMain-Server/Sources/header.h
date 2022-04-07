#pragma once


#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpServer.h>

using namespace JCore;
using namespace JNetwork;

// ���� �޸𸮸� üũ
// @�ڵ� ȹ�� �ּ� : https://stackoverflow.com/questions/29174938/googletest-and-memory-leaks
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
        printf("�޸� �� �߻� : %d bytes!!!\n", unfreedBytes);
    }
    _CrtMemState memState_;
};