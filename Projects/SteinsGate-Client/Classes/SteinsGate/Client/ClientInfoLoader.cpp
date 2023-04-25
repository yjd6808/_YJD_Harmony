/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ClientInfoLoader.h"

#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JS;
USING_NS_JC;

ClientInfoLoader::ClientInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool ClientInfoLoader::load() {

	Json::Value root;
	if (!loadJson(root))
		return false;

	try {
		Json::Value clientInfoListRoot = root["client"];

		for (int i = 0; i < clientInfoListRoot.size(); ++i) {
			Value& clientRoot = clientInfoListRoot[i];
			ClientInfo* pInfo = dbg_new ClientInfo;
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

void ClientInfoLoader::readClientInfo(Json::Value& clientRoot, Out_ ClientInfo* clientInfo) {
	clientInfo->Code = clientRoot["code"].asInt();
	clientInfo->FrameSize.width = clientRoot["frame_width"].asFloat();
	clientInfo->FrameSize.height = clientRoot["frame_height"].asFloat();
	clientInfo->GameResolutionSize.width = clientRoot["game_resolution_width"].asFloat();
	clientInfo->GameResolutionSize.height = clientRoot["game_resolution_height"].asFloat();
	clientInfo->GameResolutionPolicy = (ResolutionPolicy)clientRoot["game_resolution_policy"].asInt();
	clientInfo->Resizable = clientRoot["resizable"].asBool();
	clientInfo->FullScreen = clientRoot["fullscreen"].asBool();
	
}
