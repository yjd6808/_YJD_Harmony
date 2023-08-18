/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 9:02:57 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Time.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Logger/ConsoleLogger.h>

NS_JC_BEGIN

ConsoleLogger::ConsoleLogger()
	: LoggerAbstract(&ConsoleLoggerOption::Default)
	, m_szBuffer(4096)
{}

void ConsoleLogger::Flush() {
	bool bUseLock = m_bUseLock;

	if (bUseLock) 
		m_Lock.Lock();

	auto eDefaultColor = Console::GetColor();

	if (m_szBuffer.Length() > 0) {
		printf("%s\n", m_szBuffer.Source());
		m_szBuffer.SetLength(0);
	}

	Console::SetColor(eDefaultColor);

	if (bUseLock) 
		m_Lock.Unlock();
}

void ConsoleLogger::LogVaList(Level level, const char* fmt, va_list list) {

	if (!m_pOption->EnableLog[level])
		return;

	bool bUseLock = m_bUseLock;

	if (bUseLock)
		m_Lock.Lock();

	String szFmt = StringUtil::Format(fmt, list);

	m_szBuffer += CreateHeader(level);
	m_szBuffer += Console::VTForeColor[GetLogColor(level)];
	m_szBuffer += szFmt;

	if (m_bAutoFlush) {
		Flush();
	}
	
	if (bUseLock) 
		m_Lock.Unlock();
}

void ConsoleLogger::Log(Level level, const char* fmt, ...) {

	if (!m_pOption->EnableLog[level])
		return;

	va_list args;
	va_start(args, fmt);
	LogVaList(level, fmt, args);
	va_end(args);
}

void ConsoleLogger::LogPlainVaList(const char* fmt, va_list list) {
	if (!m_pOption->EnablePlainLog)
		return;

	bool bUseLock = m_bUseLock;

	if (bUseLock)
		m_Lock.Lock();

	String szFmt = StringUtil::Format(fmt, list);

	m_szBuffer += Console::VTForeColor[ConsoleColor::LightGray];
	m_szBuffer += szFmt;

	if (m_bAutoFlush) {
		Flush();
	}

	if (bUseLock)
		m_Lock.Unlock();
}

void ConsoleLogger::LogPlain(const char* fmt, ...) {
	if (!m_pOption->EnablePlainLog)
		return;

	va_list args;
	va_start(args, fmt);
	LogPlainVaList(fmt, args);
	va_end(args);
}

void ConsoleLogger::LogPlain(const JCore::String& str) {
	if (!m_pOption->EnablePlainLog)
		return;

	bool bUseLock = m_bUseLock;

	if (bUseLock)
		m_Lock.Lock();

	m_szBuffer += Console::VTForeColor[ConsoleColor::LightGray];
	m_szBuffer += str;

	if (m_bAutoFlush) {
		Flush();
	}

	if (bUseLock)
		m_Lock.Unlock();
}


String ConsoleLogger::CreateHeader(Level level) {

	if (m_szHeaderFormat.Length() == 0) {
		return {};
	}

	int iLevelIndex = m_szHeaderFormat.Find("level");
	int iDateTimeIndex = m_szHeaderFormat.Find("datetime");

	if (m_pOption->ShowLevel)
		DebugAssertMsg(iLevelIndex != -1, "헤더에 레벨 태그가 없습니다.");

	if (m_pOption->ShowDateTime)
		DebugAssertMsg(iDateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");

	String szDateTimeFmt = DateTime::Now().Format(m_szDateTimeFormat.Source());
	String szHeader(256);
	char szTempBuff[256];


	ConsoleLoggerOption* pConsoleOption = static_cast<ConsoleLoggerOption*>(m_pOption);

	szHeader += Console::VTForeColor[pConsoleOption->HeaderColors[level]];
	szHeader += m_szHeaderFormat;

	if (m_pOption->ShowLevel) {
		StringUtil::FormatBuffer(szTempBuff, 256, "%s%s%s",
			Console::VTForeColor[pConsoleOption->LevelColors[level]],
			m_szLevelText[level].Source(),
			Console::VTForeColor[pConsoleOption->HeaderColors[level]]
		);
		szHeader.ReplaceAll("level", szTempBuff);
	}

	if (m_pOption->ShowDateTime) {
		StringUtil::FormatBuffer(szTempBuff, 256, "%s%s%s",
			Console::VTForeColor[pConsoleOption->TimeColors[level]],
			szDateTimeFmt.Source(),
			Console::VTForeColor[pConsoleOption->HeaderColors[level]]
		);
		szHeader.ReplaceAll("datetime", szTempBuff);
	}

	return szHeader;
}

void ConsoleLogger::SetHeaderLevelColor(Level level, ConsoleColor color) {
	static_cast<ConsoleLoggerOption*>(m_pOption)->LevelColors[level] = color;
}

void ConsoleLogger::SetHeaderTimeColor(Level level, ConsoleColor color) {
	static_cast<ConsoleLoggerOption*>(m_pOption)->TimeColors[level] = color;
}

void ConsoleLogger::SetHeaderDefaultColor(Level level, ConsoleColor color) {
	static_cast<ConsoleLoggerOption*>(m_pOption)->HeaderColors[level] = color;
}

void ConsoleLogger::SetLogColor(Level level, ConsoleColor color) {
	static_cast<ConsoleLoggerOption*>(m_pOption)->LogColors[level] = color;
}

ConsoleColor ConsoleLogger::GetLogColor(Level level) {
	return static_cast<ConsoleLoggerOption*>(m_pOption)->LogColors[level];
}

void ConsoleLogger::SetLoggerOption(ConsoleLoggerOption* option, bool transferOwnership) {
	m_pOption = option;
	m_bOptionOwner = transferOwnership;
}

ConsoleLoggerOption ConsoleLoggerOption::Default;

ConsoleLoggerOption::ConsoleLoggerOption()
	: LevelColors{ LightGreen, Yellow, LightRed, Gray, LightGray }	// [ level : datatime ] -> 레벨 섹상을 바꿈
	, TimeColors{ Yellow, Yellow, Yellow, Yellow, Yellow }			// [ level : datatime ] -> 시간 섹상을 바꿈
	, HeaderColors{ White, White, White, White, White }				// [ level : datatime ] -> 그외 [ : ]이런 녀석들 색상을 바꿈
	, LogColors{ LightGreen, Yellow , LightRed, Gray, LightGray }
{}

ConsoleLoggerOption& ConsoleLoggerOption::operator=(const ConsoleLoggerOption& other) {
	LoggerOption::operator=(other);

	Arrays::Copy(LevelColors, other.LevelColors);
	Arrays::Copy(TimeColors, other.TimeColors);
	Arrays::Copy(HeaderColors, other.HeaderColors);
	Arrays::Copy(LogColors, other.LogColors);
	return *this;
}


NS_JC_END
