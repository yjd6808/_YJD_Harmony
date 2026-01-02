/*
 * 작성자: 윤정도
 * 생성일: 1/6/2023 1:02:27 AM
 * =====================
 * 
 */


#pragma once

#include <sge/Core.h>
#include <sg/Logo.h>

#include <sgcl/Struct.h>
#include <sgcl/Const.h>
#include <sgcl/GameCore.h>

#ifdef _DEBUG
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

