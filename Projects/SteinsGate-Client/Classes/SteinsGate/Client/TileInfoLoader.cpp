/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:08:17 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "TileInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
TileInfoLoader::TileInfoLoader(DataManagerAbstract* _pManager)
: ConfigFileLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool TileInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		Json::Value tileListRoot = root["tile"];

		for (size_t i = 0; i < tileListRoot.size(); ++i)
		{
			Value& tileRoot = tileListRoot[(Json::ArrayIndex)i];
			TileInfo* pInfo = dbg_new TileInfo;

			readTileInfo(tileRoot, pInfo);
			AddData(pInfo);
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
void TileInfoLoader::readTileInfo(Json::Value& _tileRoot, TileInfo* _pTileInfo)
{
	ImagePackManager* pPackManager = ImagePackManager::Get();
	_pTileInfo->code_ = _tileRoot["code"].asInt();
	_pTileInfo->SgaIndex = pPackManager->getPackIndex(JsonUtilEx::GetString(_tileRoot["sga"]));
	_pTileInfo->ImgIndex = pPackManager->getPack(_pTileInfo->SgaIndex)->getImgIndex(
		JsonUtilEx::GetString(_tileRoot["img"]));
	_pTileInfo->SpriteIndex = _tileRoot["index"].asInt();
}
