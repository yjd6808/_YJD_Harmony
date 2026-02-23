/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 10:43:27 PM
 * =====================
 */


#pragma once

#include <jc/Time.h>
#include <jc/Sync/SpinLock.h>
#include <jc/Container/Vector.h>

NS_JC_BEGIN

class JC_DLL Env
{
public:
	static const String& CurrentDirectory();
	static const String& ModulePath();
	static const String& ModuleFileName();	// 확장자 포함
	static const String& ModuleName();

	static _u32   TimeGetTime();
	static TimeSpan AppTime();			// 앱 실행후 시간이 얼마나 결과했는지
	static _u32	AppTimeTgt();		// 앱 실행후 시간이 얼마나 결과했는지
	static TimeSpan SystemTime();		// OS 실행후 시간이 얼마나 경과했는지
	static TimeSpan SystemTimeTgt();	// OS 실행후 시간이 얼마나 경과했는지 (TimeGetTime64)

	static void InitArgs(int _argc, char** _argv);
	static const Vector<String>& GetArgs() { return args_; }
	static int GetArgsCount() { return args_.Size(); }

private:
	static String		currentDirectory_; // a/b/c
	static String 		modulePath_;	// a/b/c/d.exe
	static String 		moduleFileName_;	// d.exe
	static String 		moduleName_;	// d

	static SpinLock		tgt64_lock_;
	static _u32		tgt64_lastTime_;
	static _u32		tgt64_highPart_;
	static Vector<String>	args_;
};

NS_END
