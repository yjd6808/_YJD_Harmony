/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:26 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/ChannelInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct ChannelInfoLoader : ConfigFileLoaderAbstract
{
public:
	ChannelInfoLoader(DataManagerAbstract* manager);
	virtual ~ChannelInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Channel; }
	bool load() override;

	static void readChannelBaseInfo(Json::Value& channelRoot, Out_ ChannelInfo* channelInfo);
};
