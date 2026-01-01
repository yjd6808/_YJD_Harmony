/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 9:02:46 PM
 * =====================
 *
 */


#pragma once

#include <jc/Utils/ConsoleColor.h>
#include <jc/Logger/LoggerAbstract.h>

NS_JC_BEGIN

class ConsoleLoggerOption;
class ConsoleLogger : public LoggerAbstract
{
public:
	ConsoleLogger();

	void Flush() override;
	void LogVaList(Level _level, const char* _pFmt, va_list _list) override;
	void Log(Level _level, const char* _pFmt, ...) override;
	void LogPlainVaList(const char* _pFmt, va_list _list) override;
	void LogPlain(const char* _pFmt, ...) override;
	void LogPlain(const String& _str) override;
	String CreateHeader(Level _level) override;

	void SetHeaderLevelColor(Level _level, ConsoleColor _color);
	void SetHeaderTimeColor(Level _level, ConsoleColor _color);
	void SetHeaderDefaultColor(Level _level, ConsoleColor _color);
	void SetLogColor(Level _level, ConsoleColor _color);
	ConsoleColor GetLogColor(Level _level);
	void SetLoggerOption(ConsoleLoggerOption* _pOption, bool _transferOwnership);

	LoggerType GetType() const override
	{
		return LoggerType::Console;
	}

private:
	String m_szBuffer;
};

class ConsoleLoggerOption final : public LoggerOption
{
public:
	ConsoleLoggerOption();

	ConsoleLoggerOption(const ConsoleLoggerOption& _other)
	{
		this->operator=(_other);
	}

	ConsoleLoggerOption& operator=(const ConsoleLoggerOption& _other);

	LoggerType GetLoggerType() const override
	{
		return LoggerType::Console;
	}

	ConsoleColor LevelColors[LoggerAbstract::eMax];
	ConsoleColor TimeColors[LoggerAbstract::eMax];
	ConsoleColor HeaderColors[LoggerAbstract::eMax];
	ConsoleColor LogColors[LoggerAbstract::eMax];

	static ConsoleLoggerOption Default;
};

NS_JC_END
