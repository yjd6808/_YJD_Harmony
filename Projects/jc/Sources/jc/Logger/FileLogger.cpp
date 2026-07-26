/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:15:35 PM
 * =====================
 *
 */


#include "jc/Time.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Logger/FileLogger.h"
#include "jc/Wrapper/CRuntime.h"

NS_JC_BEGIN

FileLogger::FileLogger(const String& _filePath)
: LoggerAbstract(&FileLoggerOption::Default)
, m_szBuffer(4096)
, m_szFilePath(_filePath)
, m_hFile(nullptr)
{
	m_hFile = CRuntime::FileOpen(m_szFilePath.Source(), "a");
	jc_assert_msg(m_hFile != nullptr, "FileLogger: 파일을 열 수 없습니다. (%s)", m_szFilePath.Source());
}

FileLogger::~FileLogger()
{
	Flush();
	CloseFile();
}

void FileLogger::Flush()
{
	bool useLock = m_bUseLock;

	if (useLock)
	{
		m_Lock.Lock();
	}

	if (m_szBuffer.Length() > 0)
	{
		WriteBufferToFile();
		m_szBuffer.SetLength(0);
	}

	CRuntime::FileFlush(m_hFile);

	if (useLock)
	{
		m_Lock.Unlock();
	}
}

void FileLogger::LogVaList(Level _level, const char* _pFmt, va_list _list)
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
	m_szBuffer += fmtText;
	m_szBuffer += '\n';

	if (m_bAutoFlush)
	{
		Flush();
	}

	if (useLock)
	{
		m_Lock.Unlock();
	}
}

void FileLogger::LogPlainVaList(const char* _pFmt, va_list _list)
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

	m_szBuffer += fmtText;
	m_szBuffer += '\n';

	if (m_bAutoFlush)
	{
		Flush();
	}

	if (useLock)
	{
		m_Lock.Unlock();
	}
}

jc::String FileLogger::CreateHeader(Level _level)
{
	if (m_szHeaderFormat.Length() == 0)
	{
		return {};
	}

	return LoggerAbstract::CreateHeader(_level);
}

void FileLogger::WriteBufferToFile()
{
	if (m_hFile == nullptr) return;

	const int length = m_szBuffer.Length();

	if (length <= 0) return;

	CRuntime::FileWrite(m_szBuffer.Source(), 1, length, m_hFile);
}

void FileLogger::CloseFile()
{
	if (m_hFile != nullptr)
	{
		CRuntime::FileClose(m_hFile);
		m_hFile = nullptr;
	}
}

FileLoggerOption FileLoggerOption::Default;

NS_END
