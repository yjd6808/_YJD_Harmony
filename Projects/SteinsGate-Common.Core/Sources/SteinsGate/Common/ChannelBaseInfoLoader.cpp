/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 8:36:33 AM
 * =====================
 *
 */

#include "Core.h"
#include "ChannelBaseInfoLoader.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

ChannelBaseInfoLoader::ChannelBaseInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}


bool ChannelBaseInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Json::Value& channelInfoRootList = root["channel"];

		for (int i = 0; i < channelInfoRootList.size(); ++i) {
			Value& channelInfoRoot = channelInfoRootList[i];
			ChannelBaseInfo* pChannelInfo = dbg_new ChannelBaseInfo;
			readChannelBaseInfo(channelInfoRoot, pChannelInfo);
			addData(pChannelInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void ChannelBaseInfoLoader::readChannelBaseInfo(Json::Value& channelRoot, JCORE_OUT ChannelBaseInfo* channelInfo) {
 	channelInfo->Code = channelRoot["code"].asInt();
	channelInfo->Name = JsonUtil::getString(channelRoot["name"]);
	channelInfo->EnteranceType = (EnteranceType_t)channelRoot["enterance_type"].asInt();
}



