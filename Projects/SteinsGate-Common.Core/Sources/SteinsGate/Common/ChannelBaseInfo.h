/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:18 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct ChannelBaseInfo : ConfigDataAbstract
{
	SGString Name;
	EnteranceType_t EnteranceType;
};
