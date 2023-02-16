/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:53:06 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGAIInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


USING_NS_JC;
USING_NS_JS;

#define JsonFileName "ai.json"

bool SGAIInfoLoader::LoadAIInfo(SGHashMap<int, SGAIInfo>& aiInfoMap) {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGString path = JCore::Path::Combine(ConfigDirectoryPath_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "ai.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;
	
		Json::Value aiListRoot = root["ai"];

		for (int i = 0; i < aiListRoot.size(); ++i) {
			Value& aiRoot = aiListRoot[i];

			SGAIInfo info;
			info.Code = aiRoot["code"].asInt();
			SGJson::parseFloatNumberN(aiRoot["wander_prob"], info.WanderProbs, AIWanderDecision::Max - 1);
			SGJson::parseFloatNumberN(aiRoot["track_prob"], info.TrackProbs, AITrackDecision::Max - 1);
			SGJson::parseFloatNumberN(aiRoot["angry_prob"], info.AngryProbs, AIAngryDecision::Max - 1);
			SGJson::parseFloatNumberN(aiRoot["idle_time"], info.IdleTime, 2);
			SGJson::parseFloatNumberN(aiRoot["wander_walk_time"], info.WanderWalkTime, 2);
			SGJson::parseFloatNumberN(aiRoot["follow_walk_time"], info.FollowWalkTime, 2);
			info.ForceTrack = aiRoot["force_track"].asBool();
			info.ForceAngry = aiRoot["force_angry"].asBool();
			info.SightRadious = aiRoot["sight_radious"].asFloat();
			info.AttackRadious = aiRoot["attack_radious"].asFloat();

			// 확률 사용하기 쉽도록 변경
			for (int j = 1; j < AIWanderDecision::Max - 1; ++j) {
				info.WanderProbs[j] += info.WanderProbs[j - 1];
			}

			for (int j = 1; j < AITrackDecision::Max - 1; ++j) {
				info.TrackProbs[j] += info.TrackProbs[j - 1];
			}

			for (int j = 1; j < AIAngryDecision::Max - 1; ++j) {
				info.AngryProbs[j] += info.AngryProbs[j - 1];
			}
			
			
			aiInfoMap.Insert(info.Code, info);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", JsonFileName, ex.what());
		return false;
	}

	return true;
}
