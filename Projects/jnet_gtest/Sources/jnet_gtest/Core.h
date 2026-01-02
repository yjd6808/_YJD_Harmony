//
// pch.h
//

#pragma once

#include "gtest/gtest.h"

#include <jnet/Core.h>
#include <jnet/IPAddress.h>
#include <jnet/IPEndPoint.h>
#include <jnet/Socket.h>
#include <jnet/Winsock.h>
#include <jc/Container/Vector.h>


using namespace jc;
using namespace jnet;

#define ON        1
#define OFF       0

//출력 여부
#define Print    OFF

#define TEST_IPEndPointTest     ON
#define TEST_IPAddressTest      ON
#define TEST_ByteOrderTest      ON
#define TEST_SocketTest         ON


// 범위 메모리릭 체크
// @코드 획득 주소 : https://stackoverflow.com/questions/29174938/googletest-and-memory-leaks
class AutoMemoryLeakDetector
{
public:
    AutoMemoryLeakDetector()
    {
        _CrtMemCheckpoint(&memState_);
    }

    ~AutoMemoryLeakDetector()
    {
        _CrtMemState stateNow, stateDiff;
        _CrtMemCheckpoint(&stateNow);
        const int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);

        if (diffResult)
        {
            reportFailure(stateDiff.lSizes[1]);
            _CrtMemDumpStatistics(&stateDiff);
        }
    }

private:
    void reportFailure(unsigned int _unfreedBytes)
    {
        FAIL() << "Memory leak of " << _unfreedBytes << " byte(s) detected.";
    }

    _CrtMemState memState_;
};
