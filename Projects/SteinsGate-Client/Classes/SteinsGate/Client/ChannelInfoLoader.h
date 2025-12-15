/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:26 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/ChannelInfo.h>
#include <SteinsGate/Common/ChannelBaseInfoLoader.h>

struct ChannelInfoLoader : ChannelBaseInfoLoader
{
public:
	ChannelInfoLoader(DataManagerAbstract* _pManager);
	~ChannelInfoLoader() override = default;

	bool Load() override;

	static void ReadChannelInfo(Json::Value& _channelRoot, JCORE_OUT ChannelInfo* _pChannelInfo);
};
