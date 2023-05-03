/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 1:40:58 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Logger/LoggerAbstract.h>

extern JCore::LoggerAbstract* Logger_v;

#define _LogPlain_(fmt, ...)	do { if (Logger_v) Logger_v->LogPlain(fmt, __VA_ARGS__); } while (0)
#define _LogWarn_(fmt, ...)		do { if (Logger_v) Logger_v->Log(JCore::LoggerAbstract::eWarn,	fmt, __VA_ARGS__); } while (0)
#define _LogInfo_(fmt, ...)		do { if (Logger_v) Logger_v->Log(JCore::LoggerAbstract::eInfo,	fmt, __VA_ARGS__); } while (0)
#define _LogError_(fmt, ...)	do { if (Logger_v) Logger_v->Log(JCore::LoggerAbstract::eError,fmt, __VA_ARGS__); } while (0)
#define _LogDebug_(fmt, ...)	do { if (Logger_v) Logger_v->Log(JCore::LoggerAbstract::eDebug,fmt, __VA_ARGS__); } while (0)
#define _LogPlainIf_(cond, fmt, ...)	do { if (Logger_v && (cond)) Logger_v->LogPlain(fmt, __VA_ARGS__); } while (0)
#define _LogWarnIf_(cond, fmt, ...)		do { if (Logger_v && (cond)) Logger_v->Log(JCore::LoggerAbstract::eWarn,	fmt, __VA_ARGS__); } while (0)
#define _LogInfoIf_(cond, fmt, ...)		do { if (Logger_v && (cond)) Logger_v->Log(JCore::LoggerAbstract::eInfo,	fmt, __VA_ARGS__); } while (0)
#define _LogErrorIf_(cond, fmt, ...)	do { if (Logger_v && (cond)) Logger_v->Log(JCore::LoggerAbstract::eError,fmt, __VA_ARGS__); } while (0)
#define _LogDebugIf_(cond, fmt, ...)	do { if (Logger_v && (cond)) Logger_v->Log(JCore::LoggerAbstract::eDebug,fmt, __VA_ARGS__); } while (0)

void InitializeDefaultLogger(JCore::LoggerOption* option = nullptr);
void FinalizeDefaultLogger();

