/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGClientInfoLoader.h"

#include <SteinsGate/Client/SGJsonEx.h>

USING_NS_JS;
USING_NS_JC;

bool SGClientInfoLoader::load() {

	Json::Value root;
	if (!loadJson(root))
		return false;

	try {
		Json::Value clientInfoListRoot = root["client"];

		for (int i = 0; i < clientInfoListRoot.size(); ++i) {
			Value& clientRoot = clientInfoListRoot[i];
			SGClientInfo* pInfo = dbg_new SGClientInfo;
			readClientInfo(clientRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void SGClientInfoLoader::readClientInfo(Json::Value& clientRoot, Out_ SGClientInfo* clientInfo) {
	clientInfo->Code = clientRoot["code"].asInt();
	clientInfo->GameScale = clientRoot["game_scale"].asFloat();
	clientInfo->ResolutionWidth = clientRoot["resolution_width"].asFloat();
	clientInfo->ResolutionHeight = clientRoot["resolution_height"].asFloat();
	
}
