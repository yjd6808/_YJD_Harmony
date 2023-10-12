/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 12:58:37 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Namepsace.h>

NS_CONST_BEGIN

namespace Server
{
	// 각 서버 프로세스 별로(인증, 로비, 게임, 중앙) 고유 ID를 지정한다.
	// 게임서버의 경우 여럿이 존재할 수 있고 각 게임서버도 마찬가지로 고유 ID를 가진다.
	inline constexpr int MaxProcessId = 32;	
}

NS_CONST_END