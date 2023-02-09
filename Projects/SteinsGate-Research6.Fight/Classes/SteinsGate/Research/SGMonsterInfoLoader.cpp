/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGMonsterInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>

USING_NS_JS;
USING_NS_JC;

#define JsonFileName "monster.json"

bool SGMonsterInfoLoader::LoadMonsterInfo(SGHashMap<int, SGMonsterInfo>& monsterInfoMap) {
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

	Json::Value monsterListRoot = root["monsters"];

	for (int i = 0; i < monsterListRoot.size(); ++i) {
		Value& monterRoot = monsterListRoot[i];
		Value& animationListRoot = monterRoot["animation"];
		DebugAssertMsg(animationListRoot.size() > 0, "몬스터의 애니메이션이 없습니다.");
		SGMonsterInfo monsterInfo(animationListRoot.size());


		monsterInfo.Code = monterRoot["code"].asInt();
		monsterInfo.Name = SGJson::getString(monterRoot["kor_name"]);
		monsterInfo.HP = monterRoot["hp"].asInt();
		monsterInfo.MP = monterRoot.get("mp", 0).asInt();
		monsterInfo.PhysicalDamage = monterRoot["physical_damage"].asInt();
		monsterInfo.MagicalDamage = monterRoot["magical_damage"].asInt();
		monsterInfo.Armor = monterRoot.get("armor", 0).asInt();
		monsterInfo.MagicArmor = monterRoot.get("magic_armor", 0).asInt();
		monsterInfo.EnhancePerLevel = monterRoot["enhance_per_level"].asFloat();
		monsterInfo.MoveSpeedX = monterRoot.get("move_speed_x", 0).asInt();
		monsterInfo.MoveSpeedY = monterRoot["move_speed_y"].asFloat();
		monsterInfo.DownRecoverTime = monterRoot.get("down_recover_time", 1.0).asFloat();
		monsterInfo.Weight = monterRoot["weight"].asFloat();

		SGJson::parseThicknessInfo(monterRoot["thickness_box"], monsterInfo.ThicknessBox);

		Value& partListRoot = monterRoot["parts"];
		monsterInfo.PartsCount = partListRoot.size();
		DebugAssertMsg(monsterInfo.PartsCount > 0, "몬스터의 파츠가 없습니다.");
		for (int j = 0; j < monsterInfo.PartsCount; j++) {
			Value& partRoot = partListRoot[j];
			SGMonsterPartInfo partInfo;

			SGString npkName = SGJson::getString(partRoot["npk"]);
			SGString imgName = SGJson::getString(partRoot["img"]);
			SGImagePack* pImgPack = pPackManager->getPack(npkName);

			partInfo.ZOrder = partRoot["z_order"].asInt();
			partInfo.PackIndex = pPackManager->getPackIndex(npkName);
			partInfo.ImgIndex = pImgPack->getImgIndex(imgName);


			monsterInfo.Parts[j] = Move(partInfo);
		}

		
		for (int j = 0; j < animationListRoot.size(); j++) {
			Value& animationRoot = animationListRoot[j];
			SGAnimationInfo animationInfo(animationRoot["frames"].size());
			SGJson::parseAnimationInfo(animationRoot, animationInfo);
			monsterInfo.AnimationList.PushBack(Move(animationInfo));
		}
		 monsterInfoMap.Insert(monsterInfo.Code, Move(monsterInfo));
	}
	Log("SGMonsterInfoLoader :: 로딩완료\n");
	return true;
}
