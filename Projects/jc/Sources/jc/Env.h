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

class Env
{
public:
	static String	CurrentDirectory();
	static Int32U   TimeGetTime();
	static TimeSpan AppTime();			// 앱 실행후 시간이 얼마나 결과했는지
	static Int32U	AppTimeTgt();		// 앱 실행후 시간이 얼마나 결과했는지
	static TimeSpan SystemTime();		// OS 실행후 시간이 얼마나 경과했는지
	static TimeSpan SystemTimeTgt();	// OS 실행후 시간이 얼마나 경과했는지 (TimeGetTime64)

	static void InitArgs(int _argc, char** _argv);
	static const Vector<String>& GetArgs() { return args_; }
	static int GetArgsCount() { return args_.Size(); }

private:
	static SpinLock		tgt64_lock_;
	static Int32U		tgt64_lastTime_;
	static Int32U		tgt64_highPart_;
	static Vector<String>	args_;
};

NS_JC_END
