/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:15:35 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/String.h>
#include <JCore/Sync/RecursiveLock.h>

NS_JC_BEGIN



class LoggerOption;
class LoggerAbstract
{
public:
	using TLockGuard = RecursiveLockGuard;

	enum Level
	{
		eInfo,
		eWarn,
		eError,
		eDebug,
		eMax
	};

	LoggerAbstract(LoggerOption* option);
	virtual ~LoggerAbstract() = default;

	
	virtual void Flush() = 0;
	virtual void LogVaList(Level level, const char* fmt, va_list list) = 0;
	virtual void Log(Level level, const char* fmt, ...);
	virtual void LogPlainVaList(const char* fmt, va_list list) = 0;
	virtual void LogPlain(const char* fmt, ...);
	virtual void LogPlain(const String& str);
	virtual String CreateHeader(Level level);
	
	void Log(Level level, const String& str);
	void LogInfo(const char* fmt, ...);
	void LogWarn(const char* fmt, ...);
	void LogError(const char* fmt, ...);
	void LogDebug(const char* fmt, ...);
	
	void ShowDateTime(bool enabled);
	void ShowLevel(bool enabled);
	void ShowHeader(bool enabled);
	void SetDateTimeFormat(const String& fmt);
	void SetAutoFlush(bool enabled);
	void SetHeaderFormat(const String& fmt);		// [level | datetime] => [Info | 20:08:02]
	void SetLevelText(Level level, const String& levelText);
	void SetEnableLock(bool lockEnabled);
	void SetEnableLog(Level level, bool enabled);
	void SetEnablePlainLog(bool enabled);

	virtual void SetLoggerOption(LoggerOption* option) = 0;
	LoggerOption* GetLoggerOption() { return m_pOption; }
protected:
	bool m_bAutoFlush;
	bool m_bUseLock;

	LoggerOption* m_pOption;
	String m_szHeaderFormat;
	String m_szDateTimeFormat;
	String m_szLevelText[eMax];
	RecursiveLock m_Lock;
};

class LoggerOption
{
public:
	LoggerOption();
	LoggerOption(const LoggerOption& other) { this->operator=(other); }
	LoggerOption& operator=(const LoggerOption& other);
	virtual ~LoggerOption() = 0;

	bool ShowLevel;
	bool ShowDateTime;
	bool ShowHeader;
	bool EnableLog[LoggerAbstract::eMax];
	bool EnablePlainLog;
};

inline LoggerOption::~LoggerOption() { /* Abstract */ }


NS_JC_END