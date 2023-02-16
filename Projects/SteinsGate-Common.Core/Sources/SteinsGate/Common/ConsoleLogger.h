/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 9:02:46 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/LoggerAbstract.h>

class ConsoleLogger : public LoggerAbstract
{
public:
	ConsoleLogger();

	void Flush() override;
	void Log(Level level, const char* fmt, va_list list) override;
	void Log(Level level, const char* fmt, ...) override ;
	SGString CreateHeader(Level level) override;

	void SetHeaderLevelColor(Level level, SGConsoleColor color);
	void SetHeaderTimeColor(Level level, SGConsoleColor color);
	void SetHeaderDefaultColor(Level level, SGConsoleColor color);
	void SetLogColor(Level level, SGConsoleColor color);
	void SetEnableLock(bool lock); 
private:
	SGConsoleColor m_eLevelColors[eMax];
	SGConsoleColor m_eTimeColors[eMax];
	SGConsoleColor m_eHeaderColors[eMax];
	SGConsoleColor m_eLogColors[eMax];
	bool m_bUseLock;
	SGString m_szBuffer;
	SGRecursiveLock m_Lock;
};

