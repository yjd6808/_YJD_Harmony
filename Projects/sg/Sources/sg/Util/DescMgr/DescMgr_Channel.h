/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:26 AM
 * =====================
 *
 */

#pragma once

#include "sg/Struct/SteinsGate_Channel.h"
#include "sg/Util/DescMgr/DescMgr_ChannelBase.h"

struct ChannelInfoLoader : ChannelBaseInfoLoader
{
public:
	ChannelInfoLoader();
	~ChannelInfoLoader() override = default;

	bool Load() override;

	static void ReadChannelInfo(Json::Value& _channelRoot, OUT ChannelInfo* _pChannelInfo);
};
