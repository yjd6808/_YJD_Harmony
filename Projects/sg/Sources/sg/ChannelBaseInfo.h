/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:18 AM
 * =====================
 *
 */


#pragma once

#include <sg/DescBase.h>

struct ChannelBaseInfo : SDescBase
{
	jc::String name_;
	EnteranceType_t enteranceType_;
};
