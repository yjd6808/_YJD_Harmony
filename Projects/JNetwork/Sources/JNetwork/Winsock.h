/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>
#include <JCore/String.h>
#include <JCore/LockGuard.h>

#include <iostream>

#ifndef FORMAT_STR
	#define FORMAT_STR(fmt)  JCore::StringUtil::Format(fmt)
#endif

#ifndef DebugAssert
	#include <cassert>
	#define DebugAssert(exp, msg)	assert((exp) && msg)
#endif


namespace JNetwork {

struct Winsock
{
	static bool				Initialize(Byte highVersion, Byte lowVersion);
	static bool				IsInitialized() { return ms_bInitailized; }
	static bool				Finalize();
	static bool				IsFinalized() { return ms_bFinalized; }
	static Int32U			LastError();
	static JCore::String	LastErrorMessage();

	template <typename... Args>
	static void AssertWinsockMessage(Args&&... fmt_args) {
		if (s_consoleLock)
			s_consoleLock->Lock();

		std::cout << FORMAT_STR(static_cast<Args&&>(fmt_args)...) << "\n";
		std::cout << "메시지 : " << LastErrorMessage() << "\n";

		if (s_consoleLock)
			s_consoleLock->Unlock();
		DebugAssert(false, "");
	}

	template <typename... Args>
	static void WinsockMessage(Args&&... fmt_args) {
		if (s_consoleLock) 
			s_consoleLock->Lock();

		std::cout << FORMAT_STR(static_cast<Args&&>(fmt_args)...) << "\n";
		std::cout << "메시지 : " << LastErrorMessage() << "\n";

		if (s_consoleLock) 
			s_consoleLock->Unlock();
	}

	template <typename... Args>
	static void Message(Args&&... fmt_args) {
		if (s_consoleLock)
			s_consoleLock->Lock();

		std::cout << FORMAT_STR(static_cast<Args&&>(fmt_args)...) << "\n";

		if (s_consoleLock)
			s_consoleLock->Unlock();
	}

	static void SetMutex(JCore::CriticalSectionMutex* mtx) {
		s_consoleLock = mtx;
	}
private:
	inline static JCore::CriticalSectionMutex* s_consoleLock = nullptr;

	static bool ms_bInitailized;
	static bool ms_bFinalized;
};



}

