/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 4:43:09 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "EffectInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JS;
USING_NS_JC;

////////////////////////////////////////////////////////////////////////////////////////////////////
EffectInfoLoader::EffectInfoLoader(DataManagerAbstract* _pManager)
: ConfigFileLoaderAbstract(_pManager)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool EffectInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		Value& effectListRoot = root["effect"];

		for (size_t effectIndex = 0; effectIndex < effectListRoot.size(); ++effectIndex)
		{
			Value& effectRoot = effectListRoot[(ArrayIndex)effectIndex];
			EffectInfo* pEffectInfo = dbg_new EffectInfo();
			ReadEffectInfo(effectRoot, pEffectInfo);
			AddData(pEffectInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void EffectInfoLoader::ReadEffectInfo(Json::Value& _effectRoot, EffectInfo* _pEffectInfo)
{
	_pEffectInfo->code_ = _effectRoot["code"].asInt();
	_pEffectInfo->name_ = JsonUtilEx::GetString(_effectRoot["name"]);
	JsonUtilEx::ParseActorSpriteData(_effectRoot["actor_sprite_data"], &_pEffectInfo->pSpriteData_);
}
