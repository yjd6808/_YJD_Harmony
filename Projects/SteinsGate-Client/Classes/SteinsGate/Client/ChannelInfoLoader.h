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
	ChannelInfoLoader(DataManagerAbstract* manager);
	~ChannelInfoLoader() override = default;

	bool load() override;

	static void readChannelInfo(Json::Value& channelRoot, JCORE_OUT ChannelInfo* channelInfo);
};

