/*
 * 작성자: 윤정도
 * 생성일: 1/6/2023 1:02:27 AM
 * =====================
 * 
 */


#pragma once

#include <SteinsGate/Common/Engine.h>
#include <SteinsGate/Common/Logo.h>

#include <SteinsGate/Client/Struct.h>
#include <SteinsGate/Client/Const.h>
#include <SteinsGate/Client/GameCore.h>


#ifdef DebugMode
#define SG_LOG_TICK(_time, _fmt, ...)            \
        static float Timer;                          \
        Timer += dt;                                 \
        if (Timer >= _time)                          \
        {                                            \
            printf(_fmt, ##__VA_ARGS__);             \
            Timer = 0.0f;                            \
        }
#else
#define SG_LOG_TICK(...)
#endif
