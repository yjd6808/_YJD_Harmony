/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:53:06 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "AIInfoLoader.h"

#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JC;
USING_NS_JS;

AIInfoLoader::AIInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool AIInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Json::Value aiListRoot = root["ai"];

		for (int i = 0; i < aiListRoot.size(); ++i) {
			Value& aiRoot = aiListRoot[i];
			AIInfo* pInfo = dbg_new AIInfo;
			readAIInfo(aiRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileType(), ex.what());
		return false;
	}

	return true;

}

void AIInfoLoader::readAIInfo(Json::Value& aiRoot, JCORE_OUT AIInfo* aiInfo) {
	aiInfo->Code = aiRoot["code"].asInt();
	JsonUtilEx::parseFloatNumberN(aiRoot["wander_prob"], aiInfo->WanderProbs, AIWanderDecision::Max - 1);
	JsonUtilEx::parseFloatNumberN(aiRoot["track_prob"], aiInfo->TrackProbs, AITrackDecision::Max - 1);
	JsonUtilEx::parseFloatNumberN(aiRoot["angry_prob"], aiInfo->AngryProbs, AIAngryDecision::Max - 1);
	JsonUtilEx::parseFloatNumberN(aiRoot["idle_time"], aiInfo->IdleTime, 2);
	JsonUtilEx::parseFloatNumberN(aiRoot["wander_walk_time"], aiInfo->WanderWalkTime, 2);
	JsonUtilEx::parseFloatNumberN(aiRoot["follow_walk_time"], aiInfo->FollowWalkTime, 2);
	aiInfo->ForceTrack = aiRoot["force_track"].asBool();
	aiInfo->ForceAngry = aiRoot["force_angry"].asBool();
	aiInfo->SightRadious = aiRoot["sight_radious"].asFloat();
	aiInfo->AttackRadious = aiRoot["attack_radious"].asFloat();

	// 확률 사용하기 쉽도록 변경
	for (int j = 1; j < AIWanderDecision::Max - 1; ++j) {
		aiInfo->WanderProbs[j] += aiInfo->WanderProbs[j - 1];
	}

	for (int j = 1; j < AITrackDecision::Max - 1; ++j) {
		aiInfo->TrackProbs[j] += aiInfo->TrackProbs[j - 1];
	}

	for (int j = 1; j < AIAngryDecision::Max - 1; ++j) {
		aiInfo->AngryProbs[j] += aiInfo->AngryProbs[j - 1];
	}

}
