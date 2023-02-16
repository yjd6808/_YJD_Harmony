/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:15:35 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Core.h>


class LoggerAbstract
{
public:
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
	virtual SGString CreateHeader(Level level);
	
	void Log(Level level, const SGString& str);
	void LogInfo(const char* fmt, ...);
	void LogWarn(const char* fmt, ...);
	void LogError(const char* fmt, ...);
	void LogDebug(const char* fmt, ...);
	
	void ShowDateTime(bool enabled);
	void ShowLevel(bool enabled);
	void SetDateTimeFormat(const SGString& fmt);
	void SetAutoFlush(bool enabled);
	void SetHeaderFormat(const SGString& fmt);		// [level | datetime] => [Info | 20:08:02]
	void SetLevelText(Level level, const SGString& levelText);
	
protected:
	bool m_bAutoFlush;
	bool m_bShowLevel;
	bool m_bShowDateTime;

	SGString m_szHeaderFormat;
	SGString m_szDateTimeFormat;
	SGString m_szLevelText[eMax];
};


