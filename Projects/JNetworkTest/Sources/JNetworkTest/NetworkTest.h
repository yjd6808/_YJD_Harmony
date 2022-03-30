//
// pch.h
//

#pragma once

#include "gtest/gtest.h"

#include <JNetwork/Network.h>
#include <JNetwork/IPAddress.h>
#include <JNetwork/IPEndPoint.h>
#include <JNetwork/Socket.h>
#include <JNetwork/Winsock.h>
#include <JCore/Container/Vector.h>


using namespace JCore;
using namespace JNetwork;

#define ON		1
#define OFF		0

//출력 여부
#define Print	OFF

#define TEST_IPEndPointTest     ON
#define TEST_IPAddressTest      ON
#define TEST_ByteOrderTest      ON
#define TEST_SocketTest         ON


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