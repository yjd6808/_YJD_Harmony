/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 4:19:32 PM
 * =====================
 *
 */


#include "Const.h"

NS_CONST_BEGIN

namespace Object
{
	namespace Tank
	{
		const float MoveSpeed = 70.0f;
		const float RotationSpeed = 70.0f;
		const float FireDelay = 0.3f;			// 총 발사는 0.1초에 한번만 가능
		const float BulletSpeed = 300.0f;		// 총알 속도
	}
}

namespace Net
{
	namespace EndPoint
	{
		const char Bind[] = "0.0.0.0:12345";
		const char Connect[] = "127.0.0.1:12345";
	}
}


NS_CONST_END
