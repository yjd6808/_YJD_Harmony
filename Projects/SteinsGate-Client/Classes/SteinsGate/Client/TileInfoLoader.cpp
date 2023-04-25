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

TileInfoLoader::TileInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool TileInfoLoader::load() {

	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Json::Value tileListRoot = root["tile"];

		for (int i = 0; i < tileListRoot.size(); ++i) {
			Value& tileRoot = tileListRoot[i];
			TileInfo* pInfo = dbg_new TileInfo;

			readTileInfo(tileRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void TileInfoLoader::readTileInfo(Json::Value& tileRoot, TileInfo* tileInfo) {
	ImagePackManager* pPackManager = ImagePackManager::Get();
	tileInfo->Code = tileRoot["code"].asInt();
	tileInfo->SgaIndex = pPackManager->getPackIndex(JsonUtilEx::getString(tileRoot["sga"]));
	tileInfo->ImgIndex = pPackManager->getPack(tileInfo->SgaIndex)->getImgIndex(JsonUtilEx::getString(tileRoot["img"]));
	tileInfo->SpriteIndex = tileRoot["index"].asInt();
}
