/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 1:47:34 AM
 * =====================
 *
 */


#include "Core.h"
#include "LoggerDefine.h"

#include <SteinsGate/Common/ConsoleLogger.h>

LoggerAbstract* Logger_v;

USING_NS_JC;
USING_NS_JS;

void InitializeDefaultLogger() {
	DebugAssertMsg(Logger_v == nullptr, "이미 로거가 초기화되어 있습니다.");

	ConsoleLogger* pConsoleLogger = new ConsoleLogger;
	pConsoleLogger->SetEnableLock(true);
	pConsoleLogger->SetAutoFlush(true);
	pConsoleLogger->SetHeaderLevelColor(LoggerAbstract::eInfo, LightGreen);

	pConsoleLogger->SetHeaderTimeColor(LoggerAbstract::eDebug, Yellow);
	pConsoleLogger->SetHeaderTimeColor(LoggerAbstract::eInfo, Yellow);
	pConsoleLogger->SetHeaderTimeColor(LoggerAbstract::eError, Yellow);
	pConsoleLogger->SetHeaderTimeColor(LoggerAbstract::eWarn, Yellow);
	
	pConsoleLogger->SetHeaderFormat("[ level ✓  datetime ] ");

	pConsoleLogger->ShowDateTime(true);
	pConsoleLogger->ShowLevel(true);
	Logger_v = pConsoleLogger;
}

void FinalizeDefaultLogger() {
	DeleteSafe(Logger_v);
}


