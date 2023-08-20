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
	ChannelBaseInfoLoader(DataManagerAbstract* manager);
	~ChannelBaseInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Channel; }
	bool load() override;

	static void readChannelBaseInfo(Json::Value& channelRoot, JCORE_OUT ChannelBaseInfo* channelInfo);
};

