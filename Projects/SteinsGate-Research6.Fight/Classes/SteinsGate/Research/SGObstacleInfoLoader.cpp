/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:31 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGObstacleInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


USING_NS_JC;
USING_NS_JS;

#define JsonFileName "obstacle.json"

bool SGObstacleInfoLoader::LoadObstacleInfo(SGHashMap<int, SGObstacleInfo>& obstacleInfoMap) {
	SGImagePackManager* pPackManager = SGImagePackManager::getInstance();
	SGString path = JCore::Path::Combine(ConfigDirectory_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMessage(reader.is_open(), "monster.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;
	}
	catch (std::exception& ex) {
		Log(SGStringUtil::Format("%s 파싱중 오류가 발생하였습니다. %s\n", JsonFileName, ex.what()).Source());
		return false;
	}

	Json::Value obstacleListRoot = root["obstacle"];
	for (int i = 0; i < obstacleListRoot.size(); ++i) {
		Value& projectile = obstacleListRoot[i];
		Value& animationListRoot = projectile["animation"];
		DebugAssertMessage(animationListRoot.size() > 0, "애니메이션이 없는 프로젝틸입니다.");
		SGObstacleInfo info(animationListRoot.size());
		info.Code = projectile["code"].asInt();
		info.Name = SGJson::getString(projectile["name"]);
		info.NpkIndex = pPackManager->getPackIndex(SGJson::getString(projectile["npk"]));
		info.ImgIndex = pPackManager->getPack(info.NpkIndex)->getImgIndex(SGJson::getString(projectile["img"]));
		SGJson::parseThicknessInfo(projectile["thickness_box"], info.ThicknessBox);
		info.Hitable = projectile["hitable"].asBool();
		info.Colliadalble = projectile["collidable"].asBool();
		info.ZOrederable = projectile["z_orderable"].asBool();
		

		for (ArrayIndex j = 0; j < animationListRoot.size(); ++j) {
			Value& animationRoot = animationListRoot[j];
			SGAnimationInfo animationInfo(animationRoot["frames"].size());
			SGJson::parseAnimationInfo(animationRoot, animationInfo);
			info.AnimationList.PushBack(Move(animationInfo));
		}

		obstacleInfoMap.Insert(info.Code, Move(info));
	}

	Log("SGObstacleInfoLoader :: 로딩완료\n");
	return true;
}