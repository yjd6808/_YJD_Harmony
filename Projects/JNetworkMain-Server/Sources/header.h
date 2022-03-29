#pragma once


#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Socket.h>
#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Packet.h>

#include <JCore/Container/ArrayQueue.h>

using namespace JCore;
using namespace JNetwork;

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
        int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);

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