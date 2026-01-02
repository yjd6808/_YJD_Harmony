/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:18 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescBase.h>

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(EnteranceType)
Newbie,
Begin = Newbie,
Auction,
Normal,
End = Normal,
Max
JC_SENUM_MIDDLE(EnteranceType)
JC_SENUM_MIDDLE_END(EnteranceType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(ChannelDensity)
Free,
Normal,
Busy,
Full,
Max
JC_SENUM_MIDDLE(ChannelDensity)

float Density[Max]{
	25.0f,
	50.0f,
	75.0f,
	100.0f
};
static constexpr const char* Name[Max]{
	"쾌적",
	"보통",
	"혼잡",
	"포화"
};
inline const static cc::Color3B Color[Max]{
	cc::Color3B{ 34, 139, 34 }, // 쾌적
	cc::Color3B{ 237, 145, 33 }, // 보통
	cc::Color3B{ 178, 34, 34 }, // 혼잡
	cc::Color3B{ 255, 64, 169 } // 포화
};

JC_SENUM_MIDDLE_END(ChannelDensity)

//////////////////////////////////////////////////////////////////////////////////////////
struct LobbyChannelInfo
{
	int type_;
	int number_;
	ChannelDensity_t desity_;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct ChannelBaseInfo : SDescBase
{
	jc::String name_;
	EnteranceType_t enteranceType_;
};
