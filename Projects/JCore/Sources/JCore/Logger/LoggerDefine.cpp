/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 1:47:34 AM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/Logger/LoggerDefine.h>
#include <JCore/Logger/ConsoleLogger.h>

USING_NS_JC;

LoggerAbstract* Logger_v;

void InitializeDefaultLogger(const char* specifier) {
	DebugAssertMsg(Logger_v == nullptr, "이미 로거가 초기화되어 있습니다.");

	ConsoleLogger* pConsoleLogger = dbg_new ConsoleLogger;
	pConsoleLogger->SetEnableLock(true);
	pConsoleLogger->SetAutoFlush(true);
	pConsoleLogger->SetHeaderFormat(StringUtil::Format("%s[ level ✓  datetime ] ", specifier));
	pConsoleLogger->ShowDateTime(true);
	pConsoleLogger->ShowLevel(true);
	Logger_v = pConsoleLogger;
}

void FinalizeDefaultLogger() {
	JCORE_DELETE_SAFE(Logger_v);
}

