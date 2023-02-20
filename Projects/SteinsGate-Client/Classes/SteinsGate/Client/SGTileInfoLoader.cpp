/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:08:17 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGTileInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGJsonEx.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


USING_NS_JC;
USING_NS_JS;

#define JsonFileName "tile.json"

bool SGTileInfoLoader::load() {

	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Json::Value tileListRoot = root["tile"];

		for (int i = 0; i < tileListRoot.size(); ++i) {
			Value& tileRoot = tileListRoot[i];
			SGTileInfo* pInfo = dbg_new SGTileInfo;

			readTileInfo(tileRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", JsonFileName, ex.what());
		return false;
	}

	return true;
}

void SGTileInfoLoader::readTileInfo(Json::Value& tileRoot, SGTileInfo* tileInfo) {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	tileInfo->Code = tileRoot["code"].asInt();
	tileInfo->SgaIndex = pPackManager->getPackIndex(SGJsonEx::getString(tileRoot["sga"]));
	tileInfo->ImgIndex = pPackManager->getPack(tileInfo->SgaIndex)->getImgIndex(SGJsonEx::getString(tileRoot["img"]));
	tileInfo->SpriteIndex = tileRoot["index"].asInt();
}
