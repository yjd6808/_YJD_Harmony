/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:33 AM
 * =====================
 *
 */

#include "Core.h"
#include "DescMgr_ChannelBase.h"

#include <sg/_Util/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ChannelBaseInfoLoader::ChannelBaseInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ChannelBaseInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Json::Value& channelInfoRootList = root["channel"];

		for (int i = 0; i < channelInfoRootList.size(); ++i)
		{
			Value& channelInfoRoot = channelInfoRootList[i];
			ChannelBaseInfo* pChannelInfo = dbg_new ChannelBaseInfo;
			ReadChannelBaseInfo(channelInfoRoot, pChannelInfo);
			AddData(pChannelInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ChannelBaseInfoLoader::ReadChannelBaseInfo(Json::Value& _channelRoot, OUT ChannelBaseInfo* _pChannelInfo)
{
	_pChannelInfo->code_ = _channelRoot["code"].asInt();
	_pChannelInfo->name_ = JsonUtil::GetString(_channelRoot["name"]);
	_pChannelInfo->enteranceType_ = (EnteranceType_t)_channelRoot["enterance_type"].asInt();
}
