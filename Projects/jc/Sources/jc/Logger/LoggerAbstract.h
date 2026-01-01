/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:15:35 PM
 * =====================
 *
 */


#pragma once

#include <jc/Declspec.h>
#include <jc/Primitives/String.h>
#include <jc/Sync/RecursiveLock.h>

NS_JC_BEGIN

enum class LoggerType
{
	Console
};

class LoggerOption;
class JCORE_NOVTABLE LoggerAbstract
{
public:
	using TLockGuard = RecursiveLockGuard;

	enum Level
	{
		eInfo,
		eWarn,
		eError,
		eDebug,
		eNormal,
		eMax
	};

	LoggerAbstract(LoggerOption* _pOption);
	virtual ~LoggerAbstract();

	virtual void Flush() = 0;
	virtual void LogVaList(Level _level, const char* _pFmt, va_list _list) = 0;
	virtual void Log(Level _level, const char* _pFmt, ...);
	virtual void LogPlainVaList(const char* _pFmt, va_list _list) = 0;
	virtual void LogPlain(const char* _pFmt, ...);
	virtual void LogPlain(const String& _str);
	virtual String CreateHeader(Level _level);

	void Log(Level _level, const String& _str);
	void LogInfo(const char* _pFmt, ...);
	void LogWarn(const char* _pFmt, ...);
	void LogError(const char* _pFmt, ...);
	void LogDebug(const char* _pFmt, ...);
	void Lock() { m_Lock.Lock(); }
	void Unlock() { m_Lock.Unlock(); }

	void ShowDateTime(bool _enabled);
	void ShowLevel(bool _enabled);
	void ShowHeader(bool _enabled);
	void SetDateTimeFormat(const String& _fmt);
	void SetAutoFlush(bool _enabled);
	void SetHeaderFormat(const String& _fmt);        // [level | datetime] => [Info | 20:08:02]
	void SetLevelText(Level _level, const String& _levelText);
	void SetEnableLock(bool _lockEnabled);
	void SetEnableLog(Level _level, bool _enabled);
	void SetEnablePlainLog(bool _enabled);

	virtual LoggerType GetType() const = 0;
	LoggerOption* GetLoggerOption() { return m_pOption; }

protected:
	bool m_bAutoFlush;
	bool m_bUseLock;
	bool m_bOptionOwner;

	LoggerOption* m_pOption;
	String m_szHeaderFormat;
	String m_szDateTimeFormat;
	String m_szLevelText[eMax];
	RecursiveLock m_Lock;
};

class JCORE_NOVTABLE LoggerOption
{
public:
	LoggerOption();
	LoggerOption(const LoggerOption& _other) { this->operator=(_other); }
	LoggerOption& operator=(const LoggerOption& _other);
	virtual ~LoggerOption() = 0;
	virtual LoggerType GetLoggerType() const = 0;

	bool ShowLevel;
	bool ShowDateTime;
	bool ShowHeader;
	bool EnableLog[LoggerAbstract::eMax];
	bool EnablePlainLog;
};

inline LoggerOption::~LoggerOption() { /* Abstract */ }


NS_JC_END
