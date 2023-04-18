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
	ConsoleColor GetLogColor(Level level);
	void SetLoggerOption(LoggerOption* option) override;
private:
	String m_szBuffer;
};

class ConsoleLoggerOption final : public LoggerOption
{
public:
	ConsoleLoggerOption();
	ConsoleLoggerOption(const ConsoleLoggerOption& other) { this->operator=(other); }
	ConsoleLoggerOption& operator=(const ConsoleLoggerOption& other);

	ConsoleColor LevelColors[LoggerAbstract::eMax];
	ConsoleColor TimeColors[LoggerAbstract::eMax];
	ConsoleColor HeaderColors[LoggerAbstract::eMax];
	ConsoleColor LogColors[LoggerAbstract::eMax];

	static ConsoleLoggerOption Default;
};

NS_JC_END
