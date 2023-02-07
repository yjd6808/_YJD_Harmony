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
#include <JCore/Encoding/CodePage.h>
#include <JCore/Utils/Console.h>
#include <JCore/Define.h>
#include <JCore/Assert.h>

 // 22년 12월 19일 새벽 5시
 // 내가 지금까지 계속 main 함수 내에서만 메모리릭 체크를 할려고했는데.
 // 생각해보면 PCH에서 가장먼저 체크를 시작하면 된다.
 // 그러면 제일 나중에 메모리에서 해제될테니 안전하게 내가 만든 라이브러리의 메모리릭을
 // 체크할 수 있게된다.
 // 새벽3시에 깻는데 이거땜에 잠이 안와서 누워서 2시간동안 계속 고민했는데 드디어 방법을 찾았다. ㅠㅠ

NS_JC_BEGIN
	NS_DETAIL_BEGIN
	void __JCoreInitialize();
	NS_DETAIL_END

extern Int64 AppTime_v;

inline auto JCoreInitializer = [] {
	Detail::__JCoreInitialize();
	return 0;
}();

NS_JC_END

