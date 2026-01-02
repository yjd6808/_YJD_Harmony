/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 4:38:52 PM
 * =====================
 *
 */

#include <jnet/Core.h>
#include <jc/Logger/ConsoleLogger.h>
#include <jc/Primitives/StringUtil.h>

USING_NS_JC;

LoggerAbstract* NetLogger_v;

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeNetLogger(const char* _pSpecifier)
{
	jc_assert_msg(NetLogger_v == nullptr, "이미 로거가 초기화되어 있습니다.");

	ConsoleLogger* pConsoleLogger = dbg_new ConsoleLogger;
	pConsoleLogger->SetEnableLock(true);
	pConsoleLogger->SetAutoFlush(true);
	pConsoleLogger->SetHeaderFormat(StringUtil::Format("%s[ level ✓  datetime ] ", _pSpecifier));

	pConsoleLogger->ShowDateTime(true);
	pConsoleLogger->ShowLevel(true);

	NetLogger_v = pConsoleLogger;
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeNetLogger()
{
	JC_DELETE_SAFE(NetLogger_v);
}

