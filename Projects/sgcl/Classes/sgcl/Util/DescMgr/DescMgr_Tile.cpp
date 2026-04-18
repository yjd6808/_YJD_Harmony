/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:08:17 AM
 * =====================
 *
 */

#include "DescMgr_Tile.h"

#include "sgcl/Util/JsonUtilEx.h"

#include "sgcl/Game/Texture/ImagePackManager.h"

USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
TileInfoLoader::TileInfoLoader()
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

			ReadTileInfo(tileRoot, pInfo);
			AddData(pInfo);
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
void TileInfoLoader::ReadTileInfo(Json::Value& _tileRoot, OUT TileInfo* _pTileInfo)
{
	ImagePackManager* pPackManager = ImagePackManager::Get();
	_pTileInfo->code_ = _tileRoot["code"].asInt();
	_pTileInfo->sgaIndex_ = pPackManager->GetPackIndex(JsonUtilEx::GetString(_tileRoot["sga"]));
	_pTileInfo->imgIndex_ = pPackManager->GetPack(_pTileInfo->sgaIndex_)->GetImgIndex(
		JsonUtilEx::GetString(_tileRoot["img"]));
	_pTileInfo->spriteIndex_ = _tileRoot["index"].asInt();
}
