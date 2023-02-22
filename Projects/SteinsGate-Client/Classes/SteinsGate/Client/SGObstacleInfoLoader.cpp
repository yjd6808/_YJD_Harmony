/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:31 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGObstacleInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGJsonEx.h>

USING_NS_JC;
USING_NS_JS;

SGObstacleInfoLoader::SGObstacleInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool SGObstacleInfoLoader::load() {

	Json::Value root;

	if (!loadJson(root))
		return false;
	try {

		Json::Value obstacleListRoot = root["obstacle"];
		for (int i = 0; i < obstacleListRoot.size(); ++i) {
			Value& obstacleRoot = obstacleListRoot[i];
			Value& animationListRoot = obstacleRoot["animation"];
			DebugAssertMsg(animationListRoot.size() > 0, "애니메이션이 없는 옵스터클입니다.");
			SGObstacleInfo* pInfo = dbg_new SGObstacleInfo(animationListRoot.size());

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

void SGObstacleInfoLoader::readObstacleInfo(Json::Value& obstacleRoot, SGObstacleInfo* obstacleInfo) {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	obstacleInfo->Code = obstacleRoot["code"].asInt();
	obstacleInfo->Name = SGJsonEx::getString(obstacleRoot["name"]);
	obstacleInfo->SgaIndex = pPackManager->getPackIndex(SGJsonEx::getString(obstacleRoot["sga"]));
	obstacleInfo->ImgIndex = pPackManager->getPack(obstacleInfo->SgaIndex)->getImgIndex(SGJsonEx::getString(obstacleRoot["img"]));
	SGJsonEx::parseThicknessInfo(obstacleRoot["thickness_box"], obstacleInfo->ThicknessBox);
	obstacleInfo->Hitable = obstacleRoot["hitable"].asBool();
	obstacleInfo->Colliadalble = obstacleRoot["collidable"].asBool();
	obstacleInfo->ZOrederable = obstacleRoot["z_orderable"].asBool();

	Value& animationListRoot = obstacleRoot["animation"];
	for (ArrayIndex j = 0; j < animationListRoot.size(); ++j) {
		Value& animationRoot = animationListRoot[j];
		SGAnimationInfo animationInfo(animationRoot["frames"].size());
		SGJsonEx::parseAnimationInfo(animationRoot, animationInfo);
		obstacleInfo->AnimationList.PushBack(Move(animationInfo));
	}

}
