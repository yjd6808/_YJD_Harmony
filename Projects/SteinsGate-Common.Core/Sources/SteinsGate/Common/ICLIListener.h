/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 5:48:07 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/String.h>

struct ICLIListener
{
	virtual ~ICLIListener() = default;

	// 프로파게이션 여부를 반환한다.
	virtual bool OnInputProcessing(int argc, JCore::String* argv) = 0;
};
