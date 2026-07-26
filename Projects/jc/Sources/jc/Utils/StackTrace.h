/*
 * 작성자: 윤정도
 * 생성일: 7/26/2026
 * =====================
 * 스택트레이스 캡처 및 출력
 * - DbgHelp.dll을 사용하여 심볼을 리졸브
 * - 정적 메서드 PrintStack()으로 간편 사용 가능
 */


#pragma once

#include "jc/Namespace.h"

NS_JC_BEGIN

class StackTrace
{
	static constexpr int MaxFrames = 32;
public:
	StackTrace();
	~StackTrace();

	void Capture(int _skipFrames = 1);
	void Capture(CONTEXT* _pContext);
	void Print() const;
	int GetFrameCount() const { return frameCount_; }
	void* GetAddress(int _index) const { return addresses_[_index]; }

	static void PrintStack(int _skipFrames = 1);
	static void PrintStack(CONTEXT* _pContext);
	static void EnsureSymInit();
	static void WriteMinidump(CONTEXT* _pContext, DWORD _exceptionCode);

	static bool		s_bSymInit;
	static void*	s_hProcess;

private:
	int		frameCount_;
	void*	addresses_[MaxFrames];
};

NS_END
