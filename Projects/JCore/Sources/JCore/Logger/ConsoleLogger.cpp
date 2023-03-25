/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 9:02:57 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Time.h>
#include <JCore/Logger/ConsoleLogger.h>

NS_JC_BEGIN

ConsoleLogger::ConsoleLogger() 
	: m_szBuffer(4096)
	, m_eLevelColors{ White, Yellow, LightRed, LightGray }			// [ level : datatime ] -> 레벨 섹상을 바꿈
	, m_eTimeColors{ White, Yellow, LightRed, LightGray }			// [ level : datatime ] -> 시간 섹상을 바꿈
	, m_eHeaderColors{ White, White, White, White }					// [ level : datatime ] -> 그외 [ : ]이런 녀석들 색상을 바꿈
	, m_eLogColors{ Gray, Gray , Gray , Gray }
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

	if (!m_bEnableLog[level])
		return;

	bool bUseLock = m_bUseLock;

	if (bUseLock)
		m_Lock.Lock();

	String szFmt = StringUtil::Format(fmt, list);

	m_szBuffer += CreateHeader(level);
	m_szBuffer += Console::VTForeColor[m_eLogColors[level]];
	m_szBuffer += szFmt;

	if (m_bAutoFlush) {
		Flush();
	}
	
	if (bUseLock) 
		m_Lock.Unlock();
}

void ConsoleLogger::Log(Level level, const char* fmt, ...) {

	if (!m_bEnableLog[level])
		return;

	va_list args;
	va_start(args, fmt);
	LogVaList(level, fmt, args);
	va_end(args);
}

void ConsoleLogger::LogPlainVaList(const char* fmt, va_list list) {
	if (!m_bEnablePlainLog)
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
	if (!m_bEnablePlainLog)
		return;

	va_list args;
	va_start(args, fmt);
	LogPlainVaList(fmt, args);
	va_end(args);
}

void ConsoleLogger::LogPlain(const JCore::String& str) {
	if (!m_bEnablePlainLog)
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

	if (m_bShowLevel)
		DebugAssertMsg(iLevelIndex != -1, "헤더에 레벨 태그가 없습니다.");

	if (m_bShowDateTime)
		DebugAssertMsg(iDateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");

	String szDateTimeFmt = DateTime::Now().Format(m_szDateTimeFormat.Source());
	String szHeader(256);

	char szTempBuff[256];


	szHeader += Console::VTForeColor[m_eHeaderColors[level]];
	szHeader += m_szHeaderFormat;

	if (m_bShowLevel) {
		StringUtil::FormatBuffer(szTempBuff, 256, "%s%s%s",
			Console::VTForeColor[m_eLevelColors[level]],
			m_szLevelText[level].Source(),
			Console::VTForeColor[m_eHeaderColors[level]]
		);
		szHeader.ReplaceAll("level", szTempBuff);
	}

	if (m_bShowDateTime) {
		StringUtil::FormatBuffer(szTempBuff, 256, "%s%s%s",
			Console::VTForeColor[m_eTimeColors[level]],
			szDateTimeFmt.Source(),
			Console::VTForeColor[m_eHeaderColors[level]]
		);
		szHeader.ReplaceAll("datetime", szTempBuff);
	}

	return szHeader;
}

void ConsoleLogger::SetHeaderLevelColor(Level level, ConsoleColor color) {
	m_eLevelColors[level] = color;
}

void ConsoleLogger::SetHeaderTimeColor(Level level, ConsoleColor color) {
	m_eTimeColors[level] = color;
}

void ConsoleLogger::SetHeaderDefaultColor(Level level, ConsoleColor color) {
	m_eHeaderColors[level] = color;
}

void ConsoleLogger::SetLogColor(Level level, ConsoleColor color) {
	m_eLogColors[level] = color;
}


NS_JC_END