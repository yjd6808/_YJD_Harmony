/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:15:35 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/String.h>
#include <JCore/Sync/RecursiveLock.h>

class LoggerAbstract
{
public:
	using TLockGuard = JCore::RecursiveLockGuard;

	enum Level
	{
		eInfo,
		eWarn,
		eError,
		eDebug,
		eMax
	};

	LoggerAbstract();
	virtual ~LoggerAbstract() = default;

	
	virtual void Flush() = 0;
	virtual void Log(Level level, const char* fmt, va_list list) = 0;
	virtual void Log(Level level, const char* fmt, ...);
	virtual void LogPlain(const char* fmt, va_list list) = 0;
	virtual void LogPlain(const char* fmt, ...);
	virtual void LogPlain(const JCore::String& str);
	virtual JCore::String CreateHeader(Level level);
	
	void Log(Level level, const JCore::String& str);
	void LogInfo(const char* fmt, ...);
	void LogWarn(const char* fmt, ...);
	void LogError(const char* fmt, ...);
	void LogDebug(const char* fmt, ...);
	
	void ShowDateTime(bool enabled);
	void ShowLevel(bool enabled);
	void ShowHeader(bool enabled);
	void SetDateTimeFormat(const JCore::String& fmt);
	void SetAutoFlush(bool enabled);
	void SetHeaderFormat(const JCore::String& fmt);		// [level | datetime] => [Info | 20:08:02]
	void SetLevelText(Level level, const JCore::String& levelText);
	void SetEnableLock(bool lock);
protected:
	bool m_bAutoFlush;
	bool m_bShowLevel;
	bool m_bShowDateTime;
	bool m_bShowHeader;
	bool m_bUseLock;

	JCore::String m_szHeaderFormat;
	JCore::String m_szDateTimeFormat;
	JCore::String m_szLevelText[eMax];
	JCore::RecursiveLock m_Lock;
};




