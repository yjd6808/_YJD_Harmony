#pragma once

#include <JCore/Type.h>
#include <JCore/String.h>
#include <JCore/LockGuard.h>

#ifndef FORMAT_STR
	#define FORMAT_STR(fmt)  JCore::StringUtil::Format(fmt)
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
		JCore::CriticalSectionLockGuard guard(s_consoleLock);
		std::cout << FORMAT_STR(static_cast<Args&&>(fmt_args)...) << "\n";
		std::cout << "메시지 : " << LastErrorMessage() << "\n";
		DebugAssert(false, "");
	}

	template <typename... Args>
	static void WinsockMessage(Args&&... fmt_args) {
		JCore::CriticalSectionLockGuard guard(s_consoleLock);
		std::cout << FORMAT_STR(static_cast<Args&&>(fmt_args)...) << "\n";
		std::cout << "메시지 : " << LastErrorMessage() << "\n";
	}

	template <typename... Args>
	static void Message(Args&&... fmt_args) {
		JCore::CriticalSectionLockGuard guard(s_consoleLock);
		std::cout << FORMAT_STR(static_cast<Args&&>(fmt_args)...) << "\n";
	}

private:
	static JCore::CriticalSectionMutex s_consoleLock;

	static bool ms_bInitailized;
	static bool ms_bFinalized;
};



}

