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

//////////////////////////////////////////////////////////////////////////////////////////
ClientInfoLoader::ClientInfoLoader(DataManagerAbstract* _pManager)
: ConfigFileLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ConfigFileType_t ClientInfoLoader::GetConfigFileType()
{
	return ConfigFileType::Client;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ClientInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		Json::Value clientInfoListRoot = root["client"];

		for (size_t i = 0; i < clientInfoListRoot.size(); ++i)
		{
			Value& clientRoot = clientInfoListRoot[(ArrayIndex)i];
			ClientInfo* pClientInfo = dbg_new ClientInfo;
			ReadClientInfo(clientRoot, pClientInfo);
			AddData(pClientInfo);
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
void ClientInfoLoader::ReadClientInfo(Json::Value& _clientRoot, OUT ClientInfo* _pClientInfo)
{
	_pClientInfo->code_ = _clientRoot["code"].asInt();
	_pClientInfo->frameSize_.width = _clientRoot["frame_width"].asFloat();
	_pClientInfo->frameSize_.height = _clientRoot["frame_height"].asFloat();
	_pClientInfo->gameResolutionSize_.width = _clientRoot["game_resolution_width"].asFloat();
	_pClientInfo->gameResolutionSize_.height = _clientRoot["game_resolution_height"].asFloat();
	_pClientInfo->gameResolutionPolicy_ = static_cast<ResolutionPolicy>(_clientRoot["game_resolution_policy"].asInt());
	_pClientInfo->resizable_ = _clientRoot["resizable"].asBool();
	_pClientInfo->fullScreen_ = _clientRoot["fullscreen"].asBool();

	_pClientInfo->uiScaleXFactor_ = _pClientInfo->gameResolutionSize_.width / _pClientInfo->uiResolutionSize_.width;
	_pClientInfo->uiScaleYFactor_ = _pClientInfo->gameResolutionSize_.height / _pClientInfo->uiResolutionSize_.height;
}
