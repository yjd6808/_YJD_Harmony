/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 7:36:28 AM
 * =====================
 *
 */


#include "Pch.h"
#include "Const.h"

NS_CONST_BEGIN

namespace AccessibleObject
{
	// ID 시작 인덱스
	namespace StartId
	{
		const int Player = 100'000;
		const int Room = 200'000;
	}

	namespace InitCapacity
	{
		const int Player = 1'000;
		const int Room = 1'000;
	}

	namespace InitSize
	{
		const int Player = 10;
		const int Room = 10;
	}
}

namespace Failed
{
	namespace Channel
	{
		const int AddFailedAlreadyExist	= 1;
		const int AddFailedFull = 2;
	}
}

NS_CONST_END