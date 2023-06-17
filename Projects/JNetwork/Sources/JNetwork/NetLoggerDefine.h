/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 4:39:58 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Logger/LoggerAbstract.h>

extern JCore::LoggerAbstract* NetLogger_v;

#define _NetLogPlain_(fmt, ...)		do { if (NetLogger_v) NetLogger_v->LogPlain(fmt, __VA_ARGS__); } while (0)
#define _NetLogWarn_(fmt, ...)		do { if (NetLogger_v) NetLogger_v->Log(JCore::LoggerAbstract::eWarn,	fmt, __VA_ARGS__); } while (0)
#define _NetLogInfo_(fmt, ...)		do { if (NetLogger_v) NetLogger_v->Log(JCore::LoggerAbstract::eInfo,	fmt, __VA_ARGS__); } while (0)
#define _NetLogError_(fmt, ...)		do { if (NetLogger_v) NetLogger_v->Log(JCore::LoggerAbstract::eError,	fmt, __VA_ARGS__); } while (0)
#define _NetLogDebug_(fmt, ...)		do { if (NetLogger_v) NetLogger_v->Log(JCore::LoggerAbstract::eDebug,	fmt, __VA_ARGS__); } while (0)

void InitializeNetLogger(JCore::LoggerOption* option = nullptr, const char* specifier = "");
void FinalizeNetLogger();

