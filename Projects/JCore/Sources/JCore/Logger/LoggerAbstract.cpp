/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:22:48 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Time.h>
#include <JCore/Logger/LoggerAbstract.h>

NS_JC_BEGIN

LoggerAbstract::LoggerAbstract()
	: m_bAutoFlush(false)
	, m_bShowLevel(true)
	, m_bShowDateTime(false)
	, m_szDateTimeFormat("HH:mm:ss")	//yyyy-MM-dd
	, m_szLevelText {
		"Info ",
		"Warn ",
		"Error",
		"Debug"}
	, m_bEnablePlainLog(true)
{
}

void LoggerAbstract::Log(Level level, const char* fmt, ...) {
	if (!m_bEnableLog[level])
		return;

	va_list args;
	va_start(args, fmt);
	Log(level, fmt, args);
	va_end(args);
}

void LoggerAbstract::Log(Level level, const String& str) {
	Log(level, str.Source());
}

void LoggerAbstract::LogPlain(const char* fmt, ...) {
	if (!m_bEnablePlainLog)
		return;

	bool bLock = m_bUseLock;

	if (bLock)
		m_Lock.Lock();

	bool bState = m_bShowHeader;
	ShowHeader(false);
	va_list args;
	va_start(args, fmt);
	LogPlain(fmt, args);
	va_end(args);
	ShowHeader(bState);

	if (bLock)
		m_Lock.Unlock();
}

void LoggerAbstract::LogPlain(const JCore::String& str) {
	LogPlain(str.Source());
}

void LoggerAbstract::LogInfo(const char* fmt, ...) {
	if (!m_bEnableLog[eInfo])
		return;

	va_list args;
	va_start(args, fmt);
	Log(eInfo, fmt, args);
	va_end(args);
}

void LoggerAbstract::LogWarn(const char* fmt, ...) {
	if (!m_bEnableLog[eWarn])
		return;

	va_list args;
	va_start(args, fmt);
	Log(eWarn, fmt, args);
	va_end(args);
}

void LoggerAbstract::LogError(const char* fmt, ...) {
	if (!m_bEnableLog[eError])
		return;

	va_list args;
	va_start(args, fmt);
	Log(eError, fmt, args);
	va_end(args);
}

void LoggerAbstract::LogDebug(const char* fmt, ...) {
	if (!m_bEnableLog[eDebug])
		return;

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

void LoggerAbstract::ShowHeader(bool enabled) {
	m_bShowHeader = enabled;
}

void LoggerAbstract::SetDateTimeFormat(const String& fmt) {
	m_szDateTimeFormat = fmt;
}

void LoggerAbstract::SetAutoFlush(bool enabled) {
	m_bAutoFlush = enabled;
}

void LoggerAbstract::SetEnableLock(bool lockEnabled) {
	m_bUseLock = lockEnabled;
}

void LoggerAbstract::SetEnableLog(Level level, bool enabled) {
	m_bEnableLog[level] = enabled;
}

void LoggerAbstract::SetEnablePlainLog(bool enabled) {
	m_bEnablePlainLog = enabled;
}


void LoggerAbstract::SetHeaderFormat(const String& fmt) {
	int iLevelIndex  = fmt.Find("level");
	int iDateTimeIndex = fmt.Find("datetime");

	if (m_bShowLevel)
		DebugAssertMsg(iLevelIndex != -1, "헤더에 레벨 태그가 없습니다.");
	if (m_bShowDateTime)
		DebugAssertMsg(iDateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");

	m_szHeaderFormat = fmt;
}

void LoggerAbstract::SetLevelText(Level level, const String& levelText) {
	m_szLevelText[level] = levelText;
}

String LoggerAbstract::CreateHeader(Level level) {
	int iLevelIndex = m_szHeaderFormat.Find("level");
	int iDateTimeIndex = m_szHeaderFormat.Find("datetime");

	DebugAssertMsg(m_bShowLevel && iLevelIndex != -1, "헤더에 레벨 태그가 없습니다.");
	DebugAssertMsg(m_bShowDateTime && iDateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");

	String szDateTimeFmt = DateTime::Now().Format(m_szDateTimeFormat.Source());
	String szHeader(128);

	szHeader = m_szHeaderFormat;

	if (m_bShowLevel)
		szHeader.ReplaceAll("level", m_szLevelText[level].Source());

	if (m_bShowDateTime)
		szHeader.ReplaceAll("datetime", szDateTimeFmt.Source());

	return szHeader;
}

NS_JC_END