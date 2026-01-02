/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:26 AM
 * =====================
 *
 */

#pragma once

#include <sg/ChannelBaseInfo.h>
#include <sg/_Util/DescLoaderAbstract.h>

struct ChannelBaseInfoLoader : DescLoaderAbstract
{
public:
	ChannelBaseInfoLoader(DescLoaderMgr* _pManager);
	~ChannelBaseInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Channel;
	}

	bool Load() override;

	static void ReadChannelBaseInfo(Json::Value& _channelRoot, OUT ChannelBaseInfo* _pChannelInfo);
};
