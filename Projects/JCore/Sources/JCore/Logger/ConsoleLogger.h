/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 9:02:46 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Utils/ConsoleColor.h>
#include <JCore/Logger/LoggerAbstract.h>

NS_JC_BEGIN

class ConsoleLogger : public LoggerAbstract
{
public:
	ConsoleLogger();

	void Flush() override;
	void LogVaList(Level level, const char* fmt, va_list list) override;
	void Log(Level level, const char* fmt, ...) override ;
	void LogPlainVaList(const char* fmt, va_list list) override;
	void LogPlain(const char* fmt, ...) override;
	void LogPlain(const String& str) override;
	String CreateHeader(Level level) override;

	void SetHeaderLevelColor(Level level, ConsoleColor color);
	void SetHeaderTimeColor(Level level, ConsoleColor color);
	void SetHeaderDefaultColor(Level level, ConsoleColor color);
	void SetLogColor(Level level, ConsoleColor color);
private:
	ConsoleColor m_eLevelColors[eMax];
	ConsoleColor m_eTimeColors[eMax];
	ConsoleColor m_eHeaderColors[eMax];
	ConsoleColor m_eLogColors[eMax];
	String m_szBuffer;
};

NS_JC_END
