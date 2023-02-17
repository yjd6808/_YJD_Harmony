/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 1:40:58 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/LoggerAbstract.h>

extern LoggerAbstract* Logger_v;

#define _LogPlain_(fmt, ...)	do { if (Logger_v) Logger_v->LogPlain(fmt, __VA_ARGS__); } while (0)
#define _LogWarn_(fmt, ...)		do { if (Logger_v) Logger_v->Log(LoggerAbstract::eWarn,	fmt, __VA_ARGS__); } while (0)
#define _LogInfo_(fmt, ...)		do { if (Logger_v) Logger_v->Log(LoggerAbstract::eInfo,	fmt, __VA_ARGS__); } while (0)
#define _LogError_(fmt, ...)	do { if (Logger_v) Logger_v->Log(LoggerAbstract::eError,fmt, __VA_ARGS__); } while (0)
#define _LogDebug_(fmt, ...)	do { if (Logger_v) Logger_v->Log(LoggerAbstract::eDebug,fmt, __VA_ARGS__); } while (0)

void InitializeDefaultLogger();
void FinalizeDefaultLogger();