/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:22:48 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Time.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Logger/LoggerAbstract.h>

NS_JC_BEGIN

LoggerAbstract::LoggerAbstract(LoggerOption* option)
	: m_bAutoFlush(false)
	, m_bUseLock(true)
	, m_bOptionOwner(false)
	, m_pOption(option)	//yyyy-MM-dd
	, m_szDateTimeFormat("HH:mm:ss")
	, m_szLevelText {
		"Info  ",
		"Warn  ",
		"Error ",
		"Debug ",
		"Normal"}
	
{}

LoggerAbstract::~LoggerAbstract() {
	if (m_bOptionOwner && m_pOption) {
		JCORE_DELETE_SAFE(m_pOption);
	}
}

void LoggerAbstract::Log(Level level, const char* fmt, ...) {
	if (!m_pOption->EnableLog[level])
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
	if (!m_pOption->EnablePlainLog)
		return;

	bool bLock = m_bUseLock;

	if (bLock)
		m_Lock.Lock();

	bool bState = m_pOption->ShowHeader;
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
	if (!m_pOption->EnableLog[eInfo])
		return;

	va_list args;
	va_start(args, fmt);
	Log(eInfo, fmt, args);
	va_end(args);
}

void LoggerAbstract::LogWarn(const char* fmt, ...) {
	if (!m_pOption->EnableLog[eWarn])
		return;

	va_list args;
	va_start(args, fmt);
	Log(eWarn, fmt, args);
	va_end(args);
}

void LoggerAbstract::LogError(const char* fmt, ...) {
	if (!m_pOption->EnableLog[eError])
		return;

	va_list args;
	va_start(args, fmt);
	Log(eError, fmt, args);
	va_end(args);
}

void LoggerAbstract::LogDebug(const char* fmt, ...) {
	if (!m_pOption->EnableLog[eDebug])
		return;

	va_list args;
	va_start(args, fmt);
	Log(eDebug, fmt, args);
	va_end(args);
}

void LoggerAbstract::ShowDateTime(bool enabled) {
	m_pOption->ShowDateTime = enabled;
}

void LoggerAbstract::ShowLevel(bool enabled) {
	m_pOption->ShowLevel = enabled;
}

void LoggerAbstract::ShowHeader(bool enabled) {
	m_pOption->ShowHeader = enabled;
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
	m_pOption->EnableLog[level] = enabled;
}

void LoggerAbstract::SetEnablePlainLog(bool enabled) {
	m_pOption->EnablePlainLog = enabled;
}

LoggerOption::LoggerOption()
	: ShowLevel(true)
	, ShowDateTime(true)
	, ShowHeader(true)
	, EnablePlainLog(true) {
	Arrays::Fill(EnableLog, true);
}

LoggerOption& LoggerOption::operator=(const LoggerOption& other) {
	ShowLevel = other.ShowLevel;
	ShowDateTime = other.ShowDateTime;
	ShowHeader = other.ShowHeader;
	EnablePlainLog = other.EnablePlainLog;
	Arrays::Copy(EnableLog, other.EnableLog);
	return *this;
}

void LoggerAbstract::SetHeaderFormat(const String& fmt) {
	const int iLevelIndex  = fmt.Find("level");
	const int iDateTimeIndex = fmt.Find("datetime");

	if (m_pOption->ShowLevel)
		DebugAssertMsg(iLevelIndex != -1, "헤더에 레벨 태그가 없습니다.");
	if (m_pOption->ShowDateTime)
		DebugAssertMsg(iDateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");

	m_szHeaderFormat = fmt;
}

void LoggerAbstract::SetLevelText(Level level, const String& levelText) {
	m_szLevelText[level] = levelText;
}

String LoggerAbstract::CreateHeader(Level level) {
	const int iLevelIndex = m_szHeaderFormat.Find("level");
	const int iDateTimeIndex = m_szHeaderFormat.Find("datetime");

	DebugAssertMsg(m_pOption->ShowLevel && iLevelIndex != -1, "헤더에 레벨 태그가 없습니다.");
	DebugAssertMsg(m_pOption->ShowDateTime && iDateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");

	String szDateTimeFmt = DateTime::Now().Format(m_szDateTimeFormat.Source());
	String szHeader(128);

	szHeader = m_szHeaderFormat;

	if (m_pOption->ShowLevel)
		szHeader.ReplaceAll("level", m_szLevelText[level].Source());

	if (m_pOption->ShowDateTime)
		szHeader.ReplaceAll("datetime", szDateTimeFmt.Source());

	return szHeader;
}

NS_JC_END