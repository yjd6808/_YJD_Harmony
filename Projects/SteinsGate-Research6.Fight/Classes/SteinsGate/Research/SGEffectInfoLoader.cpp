/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 4:43:09 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGEffectInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>

USING_NS_JS;
USING_NS_JC;

#define JsonFileName "effect.json"

bool SGEffectInfoLoader::LoadEffectInfo(SGHashMap<int, SGEffectInfo>& effectInfoMap) {
	SGImagePackManager* pPackManager = SGImagePackManager::getInstance();
	SGString path = JCore::Path::Combine(ConfigDirectory_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMessage(reader.is_open(), "effect.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;

		SGImagePackManager* pPackManager = SGImagePackManager::getInstance();
		SGDataManager* pDataManager = SGDataManager::getInstance();

		Value& effectListRoot = root["effect"];

		for (int i = 0; i < effectListRoot.size(); ++i) {
			Value& effectRoot = effectListRoot[i];
			 Value& animationRoot = effectRoot["animation"];

			SGEffectInfo info(animationRoot["frames"].size());
			info.Code = effectRoot["code"].asInt();
			info.Name = SGJson::getString(effectRoot["name"]);
			info.NpkIndex = pPackManager->getPackIndex(SGJson::getString(effectRoot["npk"]));
			info.ImgIndex = pPackManager->getPack(info.NpkIndex)->getImgIndex(SGJson::getString(effectRoot["img"]));
			SGJson::parseAnimationInfo(animationRoot, info.Animation);
			effectInfoMap.Insert(info.Code, Move(info));
		}

	}
	catch (std::exception& ex) {
		Log(SGStringUtil::Format("%s 파싱중 오류가 발생하였습니다. %s\n", JsonFileName, ex.what()).Source());
		return false;
	}

	
	Log("SGEffectInfoLoader :: 로딩완료\n");

	return true;
}
