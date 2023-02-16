/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:22:48 PM
 * =====================
 *
 */


#include "Core.h"
#include "LoggerAbstract.h"

LoggerAbstract::LoggerAbstract()
	: m_bAutoFlush(false)
	, m_bShowLevel(true)
	, m_bShowDateTime(false)
	, m_szDateTimeFormat("HH:mm:ss")	//yyyy-MM-dd
	, m_szLevelText {
		"Info ",
		"Warn ",
		"Error",
		"Debug"
	}
{}

void LoggerAbstract::Log(Level level, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	Log(level, fmt, args);
	va_end(args);
}

void LoggerAbstract::Log(Level level, const SGString& str) {
	Log(level, str.Source());
}

void LoggerAbstract::LogInfo(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	Log(eInfo, fmt, args);
	va_end(args);
}

void LoggerAbstract::LogWarn(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	Log(eWarn, fmt, args);
	va_end(args);
}

void LoggerAbstract::LogError(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	Log(eError, fmt, args);
	va_end(args);
}

void LoggerAbstract::LogDebug(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	Log(eDebug, fmt, args);
	va_end(args);
}

void LoggerAbstract::ShowDateTime(bool enabled) {
	m_bShowDateTime = enabled;
}

void LoggerAbstract::ShowLevel(bool enabled) {
	m_bShowLevel = enabled;
}

void LoggerAbstract::SetDateTimeFormat(const SGString& fmt) {
	m_szDateTimeFormat = fmt;
}

void LoggerAbstract::SetAutoFlush(bool enabled) {
	m_bAutoFlush = enabled;
}

void LoggerAbstract::SetHeaderFormat(const SGString& fmt) {
	int iLevelIndex  = fmt.Find("level");
	int iDateTimeIndex = fmt.Find("datetime");

	if (m_bShowLevel)
		DebugAssertMsg(iLevelIndex != -1, "헤더에 레벨 태그가 없습니다.");
	if (m_bShowDateTime)
		DebugAssertMsg(iDateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");

	m_szHeaderFormat = fmt;
}

void LoggerAbstract::SetLevelText(Level level, const SGString& levelText) {
	m_szLevelText[level] = levelText;
}

SGString LoggerAbstract::CreateHeader(Level level) {
	int iLevelIndex = m_szHeaderFormat.Find("level");
	int iDateTimeIndex = m_szHeaderFormat.Find("datetime");

	DebugAssertMsg(m_bShowLevel && iLevelIndex != -1, "헤더에 레벨 태그가 없습니다.");
	DebugAssertMsg(m_bShowDateTime && iDateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");

	SGString szDateTimeFmt = SGDateTime::Now().Format(m_szDateTimeFormat.Source());
	SGString szHeader(128);

	szHeader = m_szHeaderFormat;

	if (m_bShowLevel)
		szHeader.ReplaceAll("level", m_szLevelText[level].Source());

	if (m_bShowDateTime)
		szHeader.ReplaceAll("datetime", szDateTimeFmt.Source());

	return szHeader;
}
