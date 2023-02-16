/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 9:02:57 PM
 * =====================
 *
 */

#include "Core.h"
#include "ConsoleLogger.h"

USING_NS_JC;

ConsoleLogger::ConsoleLogger() 
	: m_szBuffer(4096)
	, m_eLevelColors{ White, Yellow, LightRed, LightGray }			// [ level : datatime ] -> 레벨 섹상을 바꿈
	, m_eTimeColors{ White, Yellow, LightRed, LightGray }			// [ level : datatime ] -> 시간 섹상을 바꿈
	, m_eHeaderColors{ White, White, White, White }					// [ level : datatime ] -> 그외 [ : ]이런 녀석들 색상을 바꿈
	, m_eLogColors{ Gray, Gray , Gray , Gray }
	, m_bUseLock(true)
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

void ConsoleLogger::Log(Level level, const char* fmt, va_list list) {

	bool bUseLock = m_bUseLock;

	if (bUseLock)
		m_Lock.Lock();

	char szText[512];
	vsprintf_s(szText, 512, fmt, list);

	m_szBuffer += CreateHeader(level);
	m_szBuffer += Console::GetVTForegroundColor(m_eLogColors[level]);
	m_szBuffer += szText;

	if (m_bAutoFlush) {
		Flush();
	}
	
	if (bUseLock) 
		m_Lock.Unlock();
}

void ConsoleLogger::Log(Level level, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	Log(level, fmt, args);
	va_end(args);
}


SGString ConsoleLogger::CreateHeader(Level level) {

	if (m_szHeaderFormat.Length() == 0) {
		return {};
	}

	int iLevelIndex = m_szHeaderFormat.Find("level");
	int iDateTimeIndex = m_szHeaderFormat.Find("datetime");

	if (m_bShowLevel)
		DebugAssertMsg(iLevelIndex != -1, "헤더에 레벨 태그가 없습니다.");

	if (m_bShowDateTime)
		DebugAssertMsg(iDateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");

	SGString szDateTimeFmt = SGDateTime::Now().Format(m_szDateTimeFormat.Source());
	SGString szHeader(256);

	char szTempBuff[256];


	szHeader += Console::GetVTForegroundColor(m_eHeaderColors[level]);
	szHeader += m_szHeaderFormat;

	if (m_bShowLevel) {
		StringUtil::FormatBuffer(szTempBuff, 256, "%s%s%s", 
			Console::GetVTForegroundColor(m_eLevelColors[level]), 
			m_szLevelText[level].Source(),
			Console::GetVTForegroundColor(m_eHeaderColors[level])
		);
		szHeader.ReplaceAll("level", szTempBuff);
	}

	if (m_bShowDateTime) {
		StringUtil::FormatBuffer(szTempBuff, 256, "%s%s%s", 
			Console::GetVTForegroundColor(m_eTimeColors[level]), 
			szDateTimeFmt.Source(),
			Console::GetVTForegroundColor(m_eHeaderColors[level])
		);
		szHeader.ReplaceAll("datetime", szTempBuff);
	}

	return szHeader;
}

void ConsoleLogger::SetHeaderLevelColor(Level level, SGConsoleColor color) {
	m_eLevelColors[level] = color;
}

void ConsoleLogger::SetHeaderTimeColor(Level level, SGConsoleColor color) {
	m_eTimeColors[level] = color;
}

void ConsoleLogger::SetHeaderDefaultColor(Level level, SGConsoleColor color) {
	m_eHeaderColors[level] = color;
}

void ConsoleLogger::SetLogColor(Level level, SGConsoleColor color) {
	m_eLogColors[level] = color;
}

void ConsoleLogger::SetEnableLock(bool enable) {
	m_bUseLock = enable;
}
