/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGMobInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGJsonEx.h>

USING_NS_JS;
USING_NS_JC;

SGMobInfoLoader::SGMobInfoLoader(DataManagerAbstract* manager)
	: MobBaseInfoLoader(manager)
{}

bool SGMobInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {

		Json::Value monsterListRoot = root["monsters"];

		for (int i = 0; i < monsterListRoot.size(); ++i) {
			Value& monterRoot = monsterListRoot[i];
			Value& animationListRoot = monterRoot["animation"];
			DebugAssertMsg(animationListRoot.size() > 0, "몬스터의 애니메이션이 없습니다.");
			SGMobInfo* monsterInfo = dbg_new SGMobInfo(animationListRoot.size());

			readMobBaseInfo(monterRoot, monsterInfo);
			readMobInfo(monterRoot, monsterInfo);
			addData(monsterInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void SGMobInfoLoader::readMobInfo(Json::Value& mobRoot, SGMobInfo* mobInfo) {

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	Value& partListRoot = mobRoot["parts"];
	mobInfo->PartsCount = partListRoot.size();
	DebugAssertMsg(mobInfo->PartsCount > 0, "몬스터의 파츠가 없습니다.");
	for (int j = 0; j < mobInfo->PartsCount; j++) {
		Value& partRoot = partListRoot[j];
		SGMobPartInfo partInfo;

		SGString sgaName = SGJsonEx::getString(partRoot["sga"]);
		SGString imgName = SGJsonEx::getString(partRoot["img"]);
		SGImagePack* pImgPack = pPackManager->getPack(sgaName);

		partInfo.ZOrder = partRoot["z_order"].asInt();
		partInfo.SgaIndex = pPackManager->getPackIndex(sgaName);
		partInfo.ImgIndex = pImgPack->getImgIndex(imgName);


		mobInfo->Parts[j] = Move(partInfo);
	}

	Value& animationListRoot = mobRoot["animation"];
	for (int j = 0; j < animationListRoot.size(); j++) {
		Value& animationRoot = animationListRoot[j];
		SGAnimationInfo animationInfo(animationRoot["frames"].size());
		SGJsonEx::parseAnimationInfo(animationRoot, animationInfo);
		mobInfo->AnimationList.PushBack(Move(animationInfo));
	}
}
