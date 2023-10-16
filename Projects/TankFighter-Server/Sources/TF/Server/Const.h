/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 7:36:20 AM
 * =====================
 *
 */


#pragma once


#include <TF/Common/Const.h>

NS_CONST_BEGIN

namespace Database
{
	namespace Game
	{
		
	}
}


namespace AccessibleObject
{
	// ID 시작 인덱스
	namespace StartId
	{
		extern const int Player;
		extern const int Room;
		extern const int Character;
	}

	namespace InitCapacity
	{
		extern const int Player;
		extern const int Room;
		extern const int Character;
	}

	namespace InitSize
	{
		extern const int Player;
		extern const int Room;
		extern const int Character;
	}
}

namespace Failed
{
	namespace Channel
	{
		extern const int AddFailedAlreadyExist;
		extern const int AddFailedFull;
	}
}

NS_CONST_END