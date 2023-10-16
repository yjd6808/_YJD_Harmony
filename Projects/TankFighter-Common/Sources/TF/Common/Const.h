/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 3:57:39 PM
 * =====================
 *
 */


#pragma once

#include <TF/Common/Namespace.h>

NS_CONST_BEGIN

constexpr int InvalidValue = -1;
constexpr int Success = 0;

namespace Length
{
	constexpr int Id = 40;
	constexpr int Pass = 40;
	constexpr int Name = 40;
	constexpr int Reason = 150;
	constexpr int Message = 300;
}

namespace Net
{
	namespace Server
	{
		extern const char GameBindEndPoint[];
		extern const char GameConnectEndPoint[];
	}
}

namespace Channel
{
	extern const int PoolSize;			// 가능한 최대 채널 갯수
}

namespace Room
{
	constexpr int MaxPlayerCount = 4;
}

namespace Map
{
	extern const float Width;
	extern const float Height;
}

namespace Tank
{
	extern const float MoveSpeed;
	extern const float RotationSpeed;
	extern const float FireDelay;
	extern const float BulletSpeed;
}






NS_CONST_END
