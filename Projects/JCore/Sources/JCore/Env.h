/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 10:43:27 PM
 * =====================
 */


#pragma once

#include <JCore/Time.h>
#include <JCore/Sync/SpinLock.h>

NS_JC_BEGIN

class Env
{
public:
	static String CurrentDirectory();
	static Int32U   TimeGetTime();
	static TimeSpan AppTime();			// 앱 실행후 시간이 얼마나 결과했는지
	static Int32U	AppTimeTGT();		// 앱 실행후 시간이 얼마나 결과했는지
	static TimeSpan SystemTime();		// OS 실행후 시간이 얼마나 경과했는지
	static TimeSpan SystemTimeTGT();	// OS 실행후 시간이 얼마나 경과했는지 (TimeGetTime64)
	

private:
	static SpinLock		tgt64_lock_;
	static Int32U		tgt64_lastTime_;
	static Int32U		tgt64_highPart_;
};


NS_JC_END
