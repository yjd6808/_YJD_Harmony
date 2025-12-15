/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:33 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "ChannelInfoLoader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ChannelInfoLoader::ChannelInfoLoader(DataManagerAbstract* _pManager)
: ChannelBaseInfoLoader(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ChannelInfoLoader::Load()
{
	Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Value& channelInfoRootList = root["channel"];

		for (int i = 0; i < channelInfoRootList.size(); ++i)
		{
			Value& channelInfoRoot = channelInfoRootList[i];
			ChannelInfo* pChannelInfo = dbg_new ChannelInfo;
			ReadChannelBaseInfo(channelInfoRoot, pChannelInfo);
			ReadChannelInfo(channelInfoRoot, pChannelInfo);
			AddData(pChannelInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ChannelInfoLoader::ReadChannelInfo(Json::Value& _channelRoot, JCORE_OUT ChannelInfo* _pChannelInfo)
{
	JsonUtil::ParseIntNumber2(
		_channelRoot["monster_sprite"],
		_pChannelInfo->SelectedSpriteIndex,
		_pChannelInfo->NormalSpriteIndex
	);
}
