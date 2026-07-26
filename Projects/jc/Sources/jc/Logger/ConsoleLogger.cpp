/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 9:02:57 PM
 * =====================
 *
 */

#include "jc/Time.h"
#include "jc/Container/Arrays.h"
#include "jc/Logger/ConsoleLogger.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleLogger::ConsoleLogger()
: LoggerAbstract(&ConsoleLoggerOption::Default)
, m_szBuffer(4096)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConsoleLogger::Flush()
{
	bool useLock = m_bUseLock;

	if (useLock)
	{
		m_Lock.Lock();
	}

	auto defaultColor = Console::GetColor();

	if (m_szBuffer.Length() > 0)
	{
		printf("%s\n", m_szBuffer.Source());
		m_szBuffer.SetLength(0);
	}

	Console::SetColor(defaultColor);

	if (useLock)
	{
		m_Lock.Unlock();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConsoleLogger::LogVaList(Level _level, const char* _pFmt, va_list _list)
{
	if (!m_pOption->EnableLog[_level])
	{
		return;
	}

	bool useLock = m_bUseLock;

	if (useLock)
	{
		m_Lock.Lock();
	}

	String fmtText = StringUtil::Format(_pFmt, _list);

	m_szBuffer += CreateHeader(_level);
	m_szBuffer += Console::VTForeColor[GetLogColor(_level)];
	m_szBuffer += fmtText;

	if (m_bAutoFlush)
	{
		Flush();
	}

	if (useLock)
	{
		m_Lock.Unlock();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConsoleLogger::LogPlainVaList(const char* _pFmt, va_list _list)
{
	if (!m_pOption->EnablePlainLog)
	{
		return;
	}

	bool useLock = m_bUseLock;

	if (useLock)
	{
		m_Lock.Lock();
	}

	String fmtText = StringUtil::Format(_pFmt, _list);

	m_szBuffer += Console::VTForeColor[ConsoleColor::LightGray];
	m_szBuffer += fmtText;

	if (m_bAutoFlush)
	{
		Flush();
	}

	if (useLock)
	{
		m_Lock.Unlock();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
String ConsoleLogger::CreateHeader(Level _level)
{
	if (m_szHeaderFormat.Length() == 0)
	{
		return {};
	}

	int levelIndex = m_szHeaderFormat.Find("level");
	int dateTimeIndex = m_szHeaderFormat.Find("datetime");

	if (m_pOption->ShowLevel)
	{
		jc_assert_msg(levelIndex != -1, "헤더에 레벨 태그가 없습니다.");
	}

	if (m_pOption->ShowDateTime)
	{
		jc_assert_msg(dateTimeIndex != -1, "헤더에 데이트타임 태그가 없습니다.");
	}

	String dateTimeFmt = DateTime::Now().Format(m_szDateTimeFormat.Source());
	String header(256);
	char tempBuff[256];

	ConsoleLoggerOption* pConsoleOption = static_cast<ConsoleLoggerOption*>(m_pOption);

	header += Console::VTForeColor[pConsoleOption->HeaderColors[_level]];
	header += m_szHeaderFormat;

	if (m_pOption->ShowLevel)
	{
		StringUtil::FormatBuffer(tempBuff, 256, "%s%s%s",
			Console::VTForeColor[pConsoleOption->LevelColors[_level]],
			m_szLevelText[_level].Source(),
			Console::VTForeColor[pConsoleOption->HeaderColors[_level]]
		);
		header.ReplaceAll("level", tempBuff);
	}

	if (m_pOption->ShowDateTime)
	{
		StringUtil::FormatBuffer(tempBuff, 256, "%s%s%s",
			Console::VTForeColor[pConsoleOption->TimeColors[_level]],
			dateTimeFmt.Source(),
			Console::VTForeColor[pConsoleOption->HeaderColors[_level]]
		);
		header.ReplaceAll("datetime", tempBuff);
	}

	return header;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConsoleLogger::SetHeaderLevelColor(Level _level, ConsoleColor _color)
{
	static_cast<ConsoleLoggerOption*>(m_pOption)->LevelColors[_level] = _color;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConsoleLogger::SetHeaderTimeColor(Level _level, ConsoleColor _color)
{
	static_cast<ConsoleLoggerOption*>(m_pOption)->TimeColors[_level] = _color;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConsoleLogger::SetHeaderDefaultColor(Level _level, ConsoleColor _color)
{
	static_cast<ConsoleLoggerOption*>(m_pOption)->HeaderColors[_level] = _color;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConsoleLogger::SetLogColor(Level _level, ConsoleColor _color)
{
	static_cast<ConsoleLoggerOption*>(m_pOption)->LogColors[_level] = _color;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleColor ConsoleLogger::GetLogColor(Level _level)
{
	return static_cast<ConsoleLoggerOption*>(m_pOption)->LogColors[_level];
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConsoleLogger::SetLoggerOption(ConsoleLoggerOption* _pOption, bool _transferOwnership)
{
	m_pOption = _pOption;
	m_bOptionOwner = _transferOwnership;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleLoggerOption ConsoleLoggerOption::Default;

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleLoggerOption::ConsoleLoggerOption()
	: LevelColors{ LightGreen, Yellow, LightRed, Gray, LightGray } // [ level : datatime ] -> 레벨 섹상을 바꿈
	, TimeColors{ Yellow, Yellow, Yellow, Yellow, Yellow } // [ level : datatime ] -> 시간 섹상을 바꿈
	, HeaderColors{ White, White, White, White, White } // [ level : datatime ] -> 그외 [ : ]이런 녀석들 색상을 바꿈
	, LogColors{ LightGreen, Yellow , LightRed, Gray, LightGray }
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleLoggerOption& ConsoleLoggerOption::operator=(const ConsoleLoggerOption& _other)
{
	LoggerOption::operator=(_other);

	Arrays::Copy(LevelColors, _other.LevelColors);
	Arrays::Copy(TimeColors, _other.TimeColors);
	Arrays::Copy(HeaderColors, _other.HeaderColors);
	Arrays::Copy(LogColors, _other.LogColors);
	return *this;
}

NS_END
