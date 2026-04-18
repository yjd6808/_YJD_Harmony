/*
 * 작성자: 윤정도
 * 생성일: 1/6/2023 1:02:27 AM
 * =====================
 * 
 */


#pragma once

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"

#include "sg/PCH/Core.h"
#include "sge/PCH/Core.h"

#include "sgcl/Core/Color.h"

#include "sgcl/Namespace.h"
#include "sgcl/Const/Const.h"
#include "sgcl/GameCore.h"

 // 코코스
namespace cocos2d
{
	using Texture = cc::Texture2D;
}

#define SGCL_MODE_NORMAL				(1)

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

