/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:26 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Common/ChannelBaseInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct ChannelBaseInfoLoader : ConfigFileLoaderAbstract
{
public:
	ChannelBaseInfoLoader(DataManagerAbstract* _pManager);
	~ChannelBaseInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Channel;
	}

	bool Load() override;

	static void ReadChannelBaseInfo(Json::Value& _channelRoot, JCORE_OUT ChannelBaseInfo* _pChannelInfo);
};
