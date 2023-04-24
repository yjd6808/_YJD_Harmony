/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:31 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "ObstacleInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JC;
USING_NS_JS;

ObstacleInfoLoader::ObstacleInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool ObstacleInfoLoader::load() {

	Json::Value root;

	if (!loadJson(root))
		return false;
	try {

		Json::Value obstacleListRoot = root["obstacle"];
		for (int i = 0; i < obstacleListRoot.size(); ++i) {
			Value& obstacleRoot = obstacleListRoot[i];
			Value& animationListRoot = obstacleRoot["animation"];
			DebugAssertMsg(animationListRoot.size() > 0, "애니메이션이 없는 옵스터클입니다.");
			ObstacleInfo* pInfo = dbg_new ObstacleInfo(animationListRoot.size());

			readObstacleInfo(obstacleRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void ObstacleInfoLoader::readObstacleInfo(Json::Value& obstacleRoot, ObstacleInfo* obstacleInfo) {
	ImagePackManager* pPackManager = ImagePackManager::get();
	obstacleInfo->Code = obstacleRoot["code"].asInt();
	obstacleInfo->Name = JsonUtilEx::getString(obstacleRoot["name"]);
	obstacleInfo->SgaIndex = pPackManager->getPackIndex(JsonUtilEx::getString(obstacleRoot["sga"]));
	obstacleInfo->ImgIndex = pPackManager->getPack(obstacleInfo->SgaIndex)->getImgIndex(JsonUtilEx::getString(obstacleRoot["img"]));
	JsonUtilEx::parseThicknessInfo(obstacleRoot["thickness_box"], obstacleInfo->ThicknessBox);
	obstacleInfo->Hitable = obstacleRoot["hitable"].asBool();
	obstacleInfo->Colliadalble = obstacleRoot["collidable"].asBool();
	obstacleInfo->ZOrederable = obstacleRoot["z_orderable"].asBool();

	Value& animationListRoot = obstacleRoot["animation"];
	for (ArrayIndex j = 0; j < animationListRoot.size(); ++j) {
		Value& animationRoot = animationListRoot[j];
		AnimationInfo animationInfo(animationRoot["frames"].size());
		JsonUtilEx::parseAnimationInfo(animationRoot, animationInfo);
		obstacleInfo->AnimationList.PushBack(Move(animationInfo));
	}

}
