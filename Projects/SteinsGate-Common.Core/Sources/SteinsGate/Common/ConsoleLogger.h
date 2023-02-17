/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 9:02:46 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Utils/ConsoleColor.h>
#include <SteinsGate/Common/LoggerAbstract.h>


class ConsoleLogger : public LoggerAbstract
{
public:
	ConsoleLogger();

	void Flush() override;
	void Log(Level level, const char* fmt, va_list list) override;
	void Log(Level level, const char* fmt, ...) override ;
	void LogPlain(const char* fmt, va_list list) override;
	void LogPlain(const char* fmt, ...) override;
	void LogPlain(const JCore::String& str) override;
	JCore::String CreateHeader(Level level) override;

	void SetHeaderLevelColor(Level level, JCore::ConsoleColor color);
	void SetHeaderTimeColor(Level level, JCore::ConsoleColor color);
	void SetHeaderDefaultColor(Level level, JCore::ConsoleColor color);
	void SetLogColor(Level level, JCore::ConsoleColor color);
private:
	JCore::ConsoleColor m_eLevelColors[eMax];
	JCore::ConsoleColor m_eTimeColors[eMax];
	JCore::ConsoleColor m_eHeaderColors[eMax];
	JCore::ConsoleColor m_eLogColors[eMax];
	JCore::String m_szBuffer;
};

