/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 7:36:20 AM
 * =====================
 *
 */


#pragma once


#include <TF/Common/Const.h>

NS_CONST_BEGIN

namespace Broadcast
{
	namespace Lobby
	{
		extern const int StateAny;
		extern const int StateLobby;
		extern const int StateMax;
	}

	namespace Room
	{
		extern const int StateAny;
	}

	namespace BattleField
	{
		extern const int StateAny;
	}
}


namespace Failed
{
	namespace Channel
	{
		extern const int AddFailedAlreadyExist;
		extern const int AddFailedFull;
		extern const int AddFailedClosed;
	}

	namespace Room
	{
		extern const int AddFailedAlreadyExist;
		extern const int AddFailedFull;
		extern const int AddFailedClosed;
	}
}

NS_CONST_END