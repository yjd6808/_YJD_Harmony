/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:22:48 PM
 * =====================
 *
 */

#include "jc/Time.h"
#include "jc/Container/Arrays.h"
#include "jc/Logger/LoggerAbstract.h"
#include "jc/Primitives/StringConvert.h"

NS_JC_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////
LoggerAbstract::LoggerAbstract(LoggerOption* _pOption)
: m_bAutoFlush(false)
, m_bUseLock(true)
, m_bOptionOwner(false)
, m_pOption(_pOption) // yyyy-MM-dd
, m_szDateTimeFormat(_T("HH:mm:ss"))
, m_szLevelText{
	_T("Info  "),
	_T("Warn  "),
	_T("Error "),
	_T("Debug "),
	_T("Normal") }
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
LoggerAbstract::~LoggerAbstract()
{
	for (int i = 0; i < m_ChainLoggers.Size(); ++i)
	{
		JC_DELETE_SAFE(m_ChainLoggers[i]);
	}
	m_ChainLoggers.Clear();

	if (m_bOptionOwner && m_pOption)
	{
		JC_DELETE_SAFE(m_pOption);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::Log(Level _level, const _char* _pFmt, ...)
{
	if (!m_pOption->EnableLog[_level])
	{
		return;
	}

	va_list args;
	va_start(args, _pFmt);

	va_list argsCopy;
	va_copy(argsCopy, args);
	LogVaList(_level, _pFmt, argsCopy);
	va_end(argsCopy);

	for (int i = 0; i < m_ChainLoggers.Size(); ++i)
	{
		va_list chainArgs;
		va_copy(chainArgs, args);
		m_ChainLoggers[i]->LogVaList(_level, _pFmt, chainArgs);
		va_end(chainArgs);
	}

	va_end(args);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::Log(Level _level, const String& _str)
{
	Log(_level, _str.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::LogPlain(const _char* _pFmt, ...)
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

	bool showHeaderState = m_pOption->ShowHeader;
	ShowHeader(false);

	va_list args;
	va_start(args, _pFmt);

	va_list argsCopy;
	va_copy(argsCopy, args);
	LogPlainVaList(_pFmt, argsCopy);
	va_end(argsCopy);

	for (int i = 0; i < m_ChainLoggers.Size(); ++i)
	{
		va_list chainArgs;
		va_copy(chainArgs, args);
		m_ChainLoggers[i]->LogPlainVaList(_pFmt, chainArgs);
		va_end(chainArgs);
	}

	va_end(args);

	ShowHeader(showHeaderState);

	if (useLock)
	{
		m_Lock.Unlock();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::LogPlain(const jc::String& _str)
{
	LogPlain(_T("%s"), _str.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::FlushAll()
{
	Flush();
	for (int i = 0; i < m_ChainLoggers.Size(); ++i)
	{
		m_ChainLoggers[i]->Flush();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::ChainLogger(LoggerAbstract* _pLogger)
{
	m_ChainLoggers.PushBack(_pLogger);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::LogInfo(const _char* _pFmt, ...)
{
	if (!m_pOption->EnableLog[eInfo])
	{
		return;
	}

	va_list args;
	va_start(args, _pFmt);
	Log(eInfo, _pFmt, args);
	va_end(args);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::LogWarn(const _char* _pFmt, ...)
{
	if (!m_pOption->EnableLog[eWarn])
	{
		return;
	}

	va_list args;
	va_start(args, _pFmt);
	Log(eWarn, _pFmt, args);
	va_end(args);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::LogError(const _char* _pFmt, ...)
{
	if (!m_pOption->EnableLog[eError])
	{
		return;
	}

	va_list args;
	va_start(args, _pFmt);
	Log(eError, _pFmt, args);
	va_end(args);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::LogDebug(const _char* _pFmt, ...)
{
	if (!m_pOption->EnableLog[eDebug])
	{
		return;
	}

	va_list args;
	va_start(args, _pFmt);
	Log(eDebug, _pFmt, args);
	va_end(args);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::ShowDateTime(bool _enabled)
{
	m_pOption->ShowDateTime = _enabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::ShowLevel(bool _enabled)
{
	m_pOption->ShowLevel = _enabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::ShowHeader(bool _enabled)
{
	m_pOption->ShowHeader = _enabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::SetDateTimeFormat(const String& _fmt)
{
	m_szDateTimeFormat = _fmt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::SetAutoFlush(bool _enabled)
{
	m_bAutoFlush = _enabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::SetEnableLock(bool _lockEnabled)
{
	m_bUseLock = _lockEnabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::SetEnableLog(Level _level, bool _enabled)
{
	m_pOption->EnableLog[_level] = _enabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::SetEnablePlainLog(bool _enabled)
{
	m_pOption->EnablePlainLog = _enabled;
}

//////////////////////////////////////////////////////////////////////////////////////////
LoggerAbstract::Level LoggerAbstract::ConvertLogLevel(const jc::String& _logLevelString)
{
	if (_logLevelString == _T("debug")) return LoggerAbstract::eDebug;
	if (_logLevelString == _T("warn")) return LoggerAbstract::eWarn;
	if (_logLevelString == _T("error")) return LoggerAbstract::eError;
	if (_logLevelString == _T("info")) return LoggerAbstract::eInfo;
	if (_logLevelString == _T("normal")) return LoggerAbstract::eInfo;
	return LoggerAbstract::eMax;
}

//////////////////////////////////////////////////////////////////////////////////////////
LoggerAbstract::Level LoggerAbstract::ConvertLogLevel(const char* _logLevelString)
{
	if (StringUtilA::IsEqual(_logLevelString, "debug")) return LoggerAbstract::eDebug;
	if (StringUtilA::IsEqual(_logLevelString, "warn")) return LoggerAbstract::eWarn;
	if (StringUtilA::IsEqual(_logLevelString, "error")) return LoggerAbstract::eError;
	if (StringUtilA::IsEqual(_logLevelString, "info")) return LoggerAbstract::eInfo;
	if (StringUtilA::IsEqual(_logLevelString, "normal")) return LoggerAbstract::eInfo;
	return LoggerAbstract::eMax;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
LoggerOption::LoggerOption()
: ShowLevel(true)
, ShowDateTime(true)
, ShowHeader(true)
, EnablePlainLog(true)
{
	Arrays::Fill(EnableLog, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
LoggerOption& LoggerOption::operator=(const LoggerOption& _other)
{
	ShowLevel = _other.ShowLevel;
	ShowDateTime = _other.ShowDateTime;
	ShowHeader = _other.ShowHeader;
	EnablePlainLog = _other.EnablePlainLog;
	Arrays::Copy(EnableLog, _other.EnableLog);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::SetHeaderFormat(const String& _fmt)
{
	const int levelIndex = _fmt.Find(_T("level"));
	const int dateTimeIndex = _fmt.Find(_T("datetime"));

	if (m_pOption->ShowLevel)
	{
		jc_assert_msg(levelIndex != -1, _T("헤더에 레벨 태그가 없습니다."));
	}

	if (m_pOption->ShowDateTime)
	{
		jc_assert_msg(dateTimeIndex != -1, _T("헤더에 데이트타임 태그가 없습니다."));
	}

	m_szHeaderFormat = _fmt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoggerAbstract::SetLevelText(Level _level, const String& _levelText)
{
	m_szLevelText[_level] = _levelText;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String LoggerAbstract::CreateHeader(Level _level)
{
	const int levelIndex = m_szHeaderFormat.Find(_T("level"));
	const int dateTimeIndex = m_szHeaderFormat.Find(_T("datetime"));

	jc_assert_msg(m_pOption->ShowLevel && levelIndex != -1, _T("헤더에 레벨 태그가 없습니다."));
	jc_assert_msg(m_pOption->ShowDateTime && dateTimeIndex != -1, _T("헤더에 데이트타임 태그가 없습니다."));

	String dateTimeFmt = DateTime::Now().Format(StringConvert::ToUtf8(m_szDateTimeFormat).Source());
	String header(128);

	header = m_szHeaderFormat;

	if (m_pOption->ShowLevel)
	{
		header.ReplaceAll(_T("level"), m_szLevelText[_level].Source());
	}

	if (m_pOption->ShowDateTime)
	{
		header.ReplaceAll(_T("datetime"), dateTimeFmt.Source());
	}

	return header;
}

NS_END
