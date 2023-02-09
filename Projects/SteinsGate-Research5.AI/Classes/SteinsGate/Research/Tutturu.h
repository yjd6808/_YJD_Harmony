﻿/*
 * 작성자: 윤정도
 * 생성일: 1/6/2023 1:02:27 AM
 * =====================
 * 
 */


#pragma once

#include <SteinsGate/Common/Engine.h>

#define Log(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define LogTick(time, fmt, ...)							\
						static float a;					\
						a += dt;						\
						if (a >= time) {				\
							printf(fmt, ##__VA_ARGS__); \
							a = 0.0f;					\
						}								