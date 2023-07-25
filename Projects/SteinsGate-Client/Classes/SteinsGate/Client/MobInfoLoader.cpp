/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "MobInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JS;
USING_NS_JC;

MobInfoLoader::MobInfoLoader(DataManagerAbstract* manager)
	: MobBaseInfoLoader(manager)
{}

bool MobInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {

		Json::Value monsterListRoot = root["monsters"];

		for (int i = 0; i < monsterListRoot.size(); ++i) {
			Value& monterRoot = monsterListRoot[i];
			MobInfo* monsterInfo = dbg_new MobInfo();
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

void MobInfoLoader::readMobInfo(Json::Value& mobRoot, MobInfo* mobInfo) {
	JsonUtilEx::parseActorSpriteData(mobRoot["actor_sprite_data"], &mobInfo->SpriteData);
	/*ImagePackManager* pPackManager = ImagePackManager::Get();

	Value& partListRoot = mobRoot["parts"];
	mobInfo->PartsCount = partListRoot.size();
	DebugAssertMsg(mobInfo->PartsCount > 0, "몬스터의 파츠가 없습니다.");
	for (int j = 0; j < mobInfo->PartsCount; j++) {
		Value& partRoot = partListRoot[j];
		ActorPartSpriteData partInfo;

		SGString sgaName = JsonUtilEx::getString(partRoot["sga"]);
		SGString imgName = JsonUtilEx::getString(partRoot["img"]);
		ImagePack* pImgPack = pPackManager->getPack(sgaName);

		partInfo.ZOrder = partRoot["z_order"].asInt();
		partInfo.SgaIndex = pPackManager->getPackIndex(sgaName);
		partInfo.ImgIndex = pImgPack->getImgIndex(imgName);


		mobInfo->Parts[j] = Move(partInfo);
	}

	Value& animationListRoot = mobRoot["animation"];
	for (int j = 0; j < animationListRoot.size(); j++) {
		Value& animationRoot = animationListRoot[j];
		AnimationInfo animationInfo(animationRoot["frames"].size());
		JsonUtilEx::parseAnimationInfo(animationRoot, animationInfo);
		mobInfo->AnimationList.PushBack(Move(animationInfo));
	}*/
}
