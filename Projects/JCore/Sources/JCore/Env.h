/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 10:43:27 PM
 * =====================
 */


#pragma once


namespace JCore {

	struct TimeSpan;

	class DateTime;
	class String;
	class Env
	{
	public:
		static String CurrentDirectory();
		static TimeSpan AppTime();
		static TimeSpan SystemTime();
	};


} // namespace JCore
