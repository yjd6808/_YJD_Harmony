/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 4:43:09 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGEffectInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGJsonEx.h>

USING_NS_JS;
USING_NS_JC;

bool SGEffectInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Value& effectListRoot = root["effect"];

		for (int i = 0; i < effectListRoot.size(); ++i) {
			Value& effectRoot = effectListRoot[i];
			int iAnimationCount = effectRoot["animation"].size();
			SGEffectInfo* pInfo = dbg_new SGEffectInfo(iAnimationCount);
			readEffectInfo(effectRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}



void SGEffectInfoLoader::readEffectInfo(Json::Value& effectRoot, SGEffectInfo* effectInfo) {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGDataManager* pDataManager = SGDataManager::get();

	Value& animationRoot = effectRoot["animation"];
	effectInfo->Code = effectRoot["code"].asInt();
	effectInfo->Name = SGJsonEx::getString(effectRoot["name"]);
	effectInfo->SgaIndex = pPackManager->getPackIndex(SGJsonEx::getString(effectRoot["sga"]));
	effectInfo->ImgIndex = pPackManager->getPack(effectInfo->SgaIndex)->getImgIndex(SGJsonEx::getString(effectRoot["img"]));
	SGJsonEx::parseAnimationInfo(animationRoot, effectInfo->Animation);
}
