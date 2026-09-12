/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 1:47:34 AM
 * =====================
 *
 */


#include "jc/Primitives/StringUtil.h"
#include "jc/Logger/LoggerDefine.h"
#include "jc/Primitives/StringConvert.h"
#include "jc/Logger/ConsoleLogger.h"
#include "jc/Logger/FileLogger.h"
#include "jc/Time.h"
#include "jc/IO/Directory.h"

USING_NS_JC;

LoggerAbstract* Logger_v;

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeDefaultLogger(const String& _pSpecifier)
{
	jc_assert_msg(Logger_v == nullptr, _T("이미 로거가 초기화되어 있습니다."));

	ConsoleLogger* pConsoleLogger = dbg_new ConsoleLogger;
	pConsoleLogger->SetEnableLock(true);
	pConsoleLogger->SetAutoFlush(true);
	pConsoleLogger->SetHeaderFormat(StringUtil::Format(_T("%s[ level ✓  datetime ] "), _pSpecifier.Source()));
	pConsoleLogger->ShowDateTime(true);
	pConsoleLogger->ShowLevel(true);
	Logger_v = pConsoleLogger;
}

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeFileLogger(const char* _pDirectory)
{
	String szTimestamp = DateTime::Now().Format("yyyy-MM-dd-HHmmss");
	String szDir = StringConvert::FromAnsi(_pDirectory);
	String szPath = StringUtil::Format(_T("%hs/%s"), _pDirectory, szTimestamp.Source());

	Directory::Create(szDir);

	FileLogger* pFileLogger = dbg_new FileLogger(szPath);
	pFileLogger->SetEnableLock(true);
	pFileLogger->SetAutoFlush(true);
	pFileLogger->SetHeaderFormat(_T("[ level | datetime ] "));
	pFileLogger->ShowDateTime(true);
	pFileLogger->ShowLevel(true);

	if (Logger_v == nullptr)
	{
		Logger_v = pFileLogger;
	}
	else
	{
		Logger_v->ChainLogger(pFileLogger);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeFileLogger(const String& _directory)
{
	String szTimestamp = DateTime::Now().Format("yyyy-MM-dd-HHmmss");
	String szPath = StringUtil::Format(_T("%s/%s"), _directory.Source(), szTimestamp.Source());

	Directory::Create(_directory);

	FileLogger* pFileLogger = dbg_new FileLogger(szPath);
	pFileLogger->SetEnableLock(true);
	pFileLogger->SetAutoFlush(true);
	pFileLogger->SetHeaderFormat(_T("[ level | datetime ] "));
	pFileLogger->ShowDateTime(true);
	pFileLogger->ShowLevel(true);

	if (Logger_v == nullptr)
	{
		Logger_v = pFileLogger;
	}
	else
	{
		Logger_v->ChainLogger(pFileLogger);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeDefaultLogger()
{
	JC_DELETE_SAFE(Logger_v);
	Logger_v = nullptr;
}
