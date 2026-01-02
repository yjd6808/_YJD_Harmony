/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 8:09:41 PM
 * =====================
 *
 */


#pragma once

#include <jc/Define.h>

JC_SENUM_BEGIN(GamePlayMode)
		Single,
		Lan,
		Wan,
		Max
	JC_SENUM_MIDDLE(GamePlayMode)

	static constexpr bool IsNetworkMode[Max]{
		false,
		true,
	};

JC_SENUM_MIDDLE_END(GamePlayMode)

// ==========================================================================================
