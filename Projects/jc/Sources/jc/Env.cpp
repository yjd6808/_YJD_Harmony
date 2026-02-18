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

#include "FileSystem/Path.h"

NS_JC_BEGIN
String			Env::currentDirectory_;
String			Env::modulePath_;
String			Env::moduleName_;

SpinLock		Env::tgt64_lock_;
Int32U			Env::tgt64_lastTime_;
Int32U			Env::tgt64_highPart_;
Vector<String>	Env::args_;

//////////////////////////////////////////////////////////////////////////////////////////
const String& Env::CurrentDirectory() 
{
	if (currentDirectory_.Length() > 0) // 멀티쓰레드 동작시 문제없도록 Config.h에서 미리 호출해놓도록 함.
		return currentDirectory_;

	char modulePath[MAX_PATH];
	int iLen = WinApi::GetModuleFilePath(NULL, modulePath, MAX_PATH);

	while (modulePath[--iLen] != '\\') {}
	if (iLen > 0) modulePath[iLen] = NULL;
	currentDirectory_ = modulePath;
	return currentDirectory_;
}

//////////////////////////////////////////////////////////////////////////////////////////
const String& Env::ModulePath()
{
	if (modulePath_.Length() > 0) // 멀티쓰레드 동작시 문제없도록 Config.h에서 미리 호출해놓도록 함.
		return modulePath_;

	char filePath[MAX_PATH];
	int iLen = WinApi::GetModuleFilePath(NULL, filePath, MAX_PATH);
	modulePath_ = filePath;
	return modulePath_;
}

//////////////////////////////////////////////////////////////////////////////////////////
const String& Env::ModuleName()
{
	if (moduleName_.Length() > 0)
		return moduleName_;

	String modulePath = ModulePath();
	moduleName_ = jc::Path::FileName(modulePath);
	return moduleName_;
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

NS_END
