/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 7:36:28 AM
 * =====================
 *
 */


#include "Pch.h"
#include "Const.h"

NS_CONST_BEGIN

namespace Broadcast
{
	namespace Lobby
	{
		const int StateAny = 0;
		const int StateLobby = 1;
		const int StateMax = 2;
	}

	namespace Room
	{
		const int StateAny = 0;
		const int StateBattle = 1;
	}

	namespace BattleField
	{
		const int StateAny = 0;
	}
}

namespace Failed
{
	namespace Channel
	{
		const int AddFailedAlreadyExist	= 1;
		const int AddFailedFull = 2;
		const int AddFailedClosed = 3;
	}

	namespace Room
	{
		const int AddFailedAlreadyExist = 1;
		const int AddFailedFull = 2;
		const int AddFailedClosed = 3;
	}
}

NS_CONST_END