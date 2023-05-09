/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 10:43:27 PM
 * =====================
 */


#pragma once


NS_JC_BEGIN

struct TimeSpan;

class DateTime;
class String;
class Env
{
public:
	static String CurrentDirectory();
	static TimeSpan AppTime();			// 앱 실행후 시간이 얼마나 결과했는지
	static TimeSpan SystemTime();		// OS 실행후 시간이 얼마나 경과했는지
};


NS_JC_END
