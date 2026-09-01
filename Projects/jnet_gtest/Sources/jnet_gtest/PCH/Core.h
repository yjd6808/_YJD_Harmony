/*
	작성자 : 윤정도
	jnet_gtest PCH 파일입니다.
*/

#pragma once

#include <gtest/gtest.h>

#include <vector>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <chrono>
#include <crtdbg.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "jc/PCH/Core.h"
#include "jc/Debug/MemoryLeakDetector.h"
#include "jc/Debug/MemoryPoolLeakDetector.h"
#include "jc/Threading/Thread.h"

#include "jnet/Namespace.h"

namespace jnet {}
USING_NS_JC;
USING_NS_JNET;
USING_NS_STD;

#define ON		1
#define OFF		0

//출력 여부
#define Print						OFF
#define TestEnabled					ON		// 전체 테스트 수행 여부

#define HttpTestEnabled				ON		// jnet::Http 테스트 수행 여부

// 개별 테스트 수행시 사용
#if TestEnabled == OFF
	#define TEST_HttpClientSyncTest				ON
#endif

#if TestEnabled == ON

	#if HttpTestEnabled == ON
		#define TEST_MockHttpServerSelfTest		ON
		#define TEST_HttpHeadersTest			ON
		#define TEST_HttpClientSyncTest			ON
		#define TEST_HttpClientAsyncTest		ON
		#define TEST_HttpDataSourceBridgeTest	ON
	#endif

#endif
// https://stackoverflow.com/questions/1082192/how-to-generate-random-variable-names-in-c-using-macros
#define LeakCheckConcat(a, b) LeakCheckConcatInner(a, b)
#define LeakCheckConcatInner(a, b) a##b {[](_u32 unfreedBytes) { FAIL() << unfreedBytes << " 바이트 메모리 릭\n"; }}
#define LeakCheck AutoMemoryLeakDetector LeakCheckConcat(LeakCheck, __COUNTER__)
