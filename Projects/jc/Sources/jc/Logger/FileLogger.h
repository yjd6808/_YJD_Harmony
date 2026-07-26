/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 8:15:35 PM
 * =====================
 *
 */


#pragma once

#include "jc/Logger/LoggerAbstract.h"

NS_JC_BEGIN

class FileLoggerOption final : public LoggerOption
{
public:
	FileLoggerOption() = default;

	FileLoggerOption(const FileLoggerOption& _other)
	{
		this->operator=(_other);
	}

	FileLoggerOption& operator=(const FileLoggerOption& _other)
	{
		LoggerOption::operator=(_other);
		return *this;
	}

	LoggerType GetLoggerType() const override
	{
		return LoggerType::File;
	}

	static FileLoggerOption Default;
};

class FileLogger final : public LoggerAbstract
{
public:
	FileLogger(const String& _filePath);
	~FileLogger() override;

	void Flush() override;
	void LogVaList(Level _level, const char* _pFmt, va_list _list) override;
	void LogPlainVaList(const char* _pFmt, va_list _list) override;
	String CreateHeader(Level _level) override;

	LoggerType GetType() const override
	{
		return LoggerType::File;
	}

	const String& GetFilePath() const { return m_szFilePath; }

private:
	void WriteBufferToFile();
	void CloseFile();

	String m_szBuffer;
	String m_szFilePath;
	_iohandle m_hFile;
};

NS_END
