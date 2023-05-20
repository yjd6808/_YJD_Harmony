/*
 *	작성자 : 윤정도
 *	PCH 파일입니다.
 */

#pragma once

#pragma warning (disable : 4267)		// conversion from 'size_t' to 'type', possible loss of data
#pragma warning (disable : 26439)		// This kind of function may not throw.Declare it 'noexcept' (f.6)
#pragma warning (disable : 4018)		// '<': signed / unsigned mismatch

// Windows.h 헤더에 Winsock.h 헤더가 포함되는 것을 막는다. 그 외에 헤더들이 많이 추가되는 것을 방지 
// https://social.msdn.microsoft.com/Forums/vstudio/en-US/6177f9bc-db75-4317-9904-02ef55b527d0/linking-problem-with-winsock2?forum=vcgeneral
#define WIN32_LEAN_AND_MEAN	

#include <iostream>
#include <thread>

#include <Windows.h>
#include <winnt.h>
#include <exception>
#include <random>

#include <JCore/Debug/MemoryLeakDetector.h>
#include <JCore/Debug/New.h>

#include <JCore/Encoding/CodePage.h>
#include <JCore/Utils/Console.h>
#include <JCore/Logger/LoggerDefine.h>
#include <JCore/Define.h>
#include <JCore/Macro.h>
#include <JCore/Assert.h>


NS_JC_BEGIN
	NS_DETAIL_BEGIN
	void InitializeJCore();
	NS_DETAIL_END

extern Int64 AppTime_v;

inline auto JCoreInitializer = [] {
	Detail::InitializeJCore();
	return 0;
}();

NS_JC_END



