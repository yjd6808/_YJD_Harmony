/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 3:57:39 PM
 * =====================
 *
 */


#pragma once

#include <TF/Common/Namespace.h>




NS_CONST_BEGIN

// ====================================================
// constexpr
// ====================================================

constexpr int InvalidValue				= -1;

constexpr int IdLength					= 40;
constexpr int PassLength				= 40;
constexpr int NameLength				= 40;
constexpr int ReasonLength				= 150;
constexpr int MessageLength				= 300;

constexpr int RoomMaxPlayerCount		= 4;


// ====================================================
// const
// ====================================================

extern const float ScreenWidth;
extern const float ScreenHeight;

extern const float MapWidth;
extern const float MapHeight;

extern const float TankMoveSpeed;
extern const float TankRotationSpeed;
extern const float TankFireDelay;
extern const float TankBulletSpeed;

NS_CONST_END
