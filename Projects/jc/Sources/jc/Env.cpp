/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 10:53:21 PM
 * =====================
 *
 */

#include <jc/Core.h>
#include <jc/Env.h>
#include <jc/Primitives/String.h>
#include <jc/Wrapper/WinApi.h>

#include <timeapi.h>

NS_JC_BEGIN

SpinLock	Env::tgt64_lock_;
Int32U		Env::tgt64_lastTime_;
Int32U		Env::tgt64_highPart_;
Vector<String>	Env::args_;

//////////////////////////////////////////////////////////////////////////////////////////
String Env::CurrentDirectory() 
{
	char szFileName[MAX_PATH];
	int iLen = WinApi::GetModuleFilePath(NULL, szFileName, MAX_PATH);

	while (szFileName[--iLen] != '\\') {}
	if (iLen > 0) szFileName[iLen] = NULL;
	return szFileName;
}

//////////////////////////////////////////////////////////////////////////////////////////
Int32U Env::TimeGetTime()
{
	return ::timeGetTime();
}

//////////////////////////////////////////////////////////////////////////////////////////
TimeSpan Env::AppTime() {
	return DateTime::Now().Diff(AppStartUpTime_v);
}

//////////////////////////////////////////////////////////////////////////////////////////
Int32U Env::AppTimeTgt()
{
	return ::timeGetTime() - AppStartUpTimeTGT_v;
}

//////////////////////////////////////////////////////////////////////////////////////////
TimeSpan Env::SystemTime() {
	return { Int64(::GetTickCount64()) * 1000 };
}

//////////////////////////////////////////////////////////////////////////////////////////
TimeSpan Env::SystemTimeTgt()
{
	SpinLockGuard lock(tgt64_lock_);

	DWORD current = ::timeGetTime();
	if (current < tgt64_lastTime_) {
		// 래핑 발생 시 상위 32비트 카운터 증가
		tgt64_highPart_++;
	}
	tgt64_lastTime_ = current;

	Int64 result = (static_cast<Int64>(tgt64_highPart_) << 32) | current;
	return { result };
}

//////////////////////////////////////////////////////////////////////////////////////////
void Env::InitArgs(int _argc, char** _argv)
{
	args_.Clear();
	for (int i = 0; i < _argc; ++i) 
	{
		args_.PushBack(String(_argv[i]));
	}
}

NS_JC_END
