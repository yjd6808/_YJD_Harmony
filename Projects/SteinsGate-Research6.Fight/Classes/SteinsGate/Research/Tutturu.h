/*
 * 작성자: 윤정도
 * 생성일: 1/6/2023 1:02:27 AM
 * =====================
 * 
 */


#pragma once

#include <SteinsGate/Common/Engine.h>

#ifdef DebugMode
	#define Log(fmt, ...) printf(fmt, ##__VA_ARGS__)
	#define LogTick(time, fmt, ...)							\
							static float __timer__;			\
							__timer__ += dt;				\
							if (__timer__ >= time) {		\
								printf(fmt, ##__VA_ARGS__); \
								__timer__ = 0.0f;			\
							}

#else
	#define Log(...)
	#define LogTick(...)
#endif
