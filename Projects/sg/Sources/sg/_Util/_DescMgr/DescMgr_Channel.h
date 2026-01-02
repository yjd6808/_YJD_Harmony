/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:26 AM
 * =====================
 *
 */

#pragma once

#include <sg/_Struct/SteinsGate_Channel.h>
#include <sg/_Util/_DescMgr/DescMgr_ChannelBase.h>

struct ChannelInfoLoader : ChannelBaseInfoLoader
{
public:
	ChannelInfoLoader();
	~ChannelInfoLoader() override = default;

	bool Load() override;

	static void ReadChannelInfo(Json::Value& _channelRoot, OUT ChannelInfo* _pChannelInfo);
};
