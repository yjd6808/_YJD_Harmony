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

NS_CONST_END
