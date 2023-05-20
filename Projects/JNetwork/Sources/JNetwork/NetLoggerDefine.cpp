﻿/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 4:38:52 PM
 * =====================
 *
 */

#include <JNetwork/Network.h>
#include <JCore/Logger/ConsoleLogger.h>

USING_NS_JC;

LoggerAbstract* NetLogger_v;

void InitializeNetLogger(LoggerOption* option) {
	DebugAssertMsg(NetLogger_v == nullptr, "이미 로거가 초기화되어 있습니다.");

	ConsoleLogger* pConsoleLogger = dbg_new ConsoleLogger;
	pConsoleLogger->SetEnableLock(true);
	pConsoleLogger->SetAutoFlush(true);
	pConsoleLogger->SetHeaderFormat("[ level ✓  datetime ] ");

	pConsoleLogger->ShowDateTime(true);
	pConsoleLogger->ShowLevel(true);

	if (option) { pConsoleLogger->SetLoggerOption(option); }
	NetLogger_v = pConsoleLogger;
}

void FinalizeNetLogger() {
	JCORE_DELETE_SAFE(NetLogger_v);
}