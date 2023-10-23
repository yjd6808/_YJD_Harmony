/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 4:19:32 PM
 * =====================
 *
 */

#include "PchBase.h"
#include "Const.h"

NS_CONST_BEGIN

namespace Net
{
	namespace Server
	{
		const char GameBindEndPoint[] = "0.0.0.0:12345";
		const char GameConnectEndPoint[] = "127.0.0.1:12345";
	}
}

namespace Channel
{
	const int PoolSize = 16;
}

namespace Room
{
	const int PoolSize = 1000;
}

namespace Tank
{
	const float BodyWidth = 30.0f;
	const float BodyHeight = 60.0f;
	const float MoveSpeed = 70.0f;
	const float RotationSpeed = 70.0f;
	const float FireDelay = 0.3f;			// 총 발사는 0.1초에 한번만 가능
	const float BulletSpeed = 300.0f;		// 총알 속도
}

namespace Map
{
	const float Width = 1000.0f;
	const float Height = 600.0f;
}

namespace BattleField
{
	// 모두 초단위
	const float PlayWaitTime = 7.0f;
	const float PlayingTime = 120.0f;
	const float EndWaitTime = 10.0f;
	const float RevivalTime = 5.0f;
}


namespace AccessibleObject
{
	// ID 시작 인덱스
	namespace StartId
	{
		const int Player = 100'000;
		const int Room = 200'000;
		const int Character = 300'000;
	}

	namespace InitCapacity
	{
		const int Player = 1'000;
		const int Room = 1'000;
		const int Character = 1'000;
	}

	namespace InitSize
	{
		const int Player = 10;
		const int Room = 10;
		const int Character = 10;
	}
}

NS_CONST_END
