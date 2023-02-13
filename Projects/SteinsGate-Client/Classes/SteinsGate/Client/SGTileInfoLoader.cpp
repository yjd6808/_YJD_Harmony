/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:08:17 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGTileInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


USING_NS_JC;
USING_NS_JS;

#define JsonFileName "tile.json"

bool SGTileInfoLoader::LoadTileInfo(SGHashMap<int, SGTileInfo>& tileInfoMap) {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGString path = JCore::Path::Combine(ConfigDirectory_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "monster.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;
	}
	catch (std::exception& ex) {
		Log(SGStringUtil::Format("%s 파싱중 오류가 발생하였습니다. %s\n", JsonFileName, ex.what()).Source());
		return false;
	}

	Json::Value tileListRoot = root["tile"];

	for (int i = 0; i < tileListRoot.size(); ++i) {
		Value& tileRoot = tileListRoot[i];

		SGTileInfo info;
		info.Code = tileRoot["code"].asInt();
		info.NpkIndex = pPackManager->getPackIndex(SGJson::getString(tileRoot["npk"]));
		info.ImgIndex = pPackManager->getPack(info.NpkIndex)->getImgIndex(SGJson::getString(tileRoot["img"]));
		info.SpriteIndex = tileRoot["index"].asInt();
		tileInfoMap.Insert(info.Code, info);
	}

	Log("SGTileInfoLoader :: 로딩완료\n");
	return true;
}
