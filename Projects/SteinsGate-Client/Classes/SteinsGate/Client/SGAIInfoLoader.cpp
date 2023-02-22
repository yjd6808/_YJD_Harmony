/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:53:06 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGAIInfoLoader.h"

#include <SteinsGate/Client/SGJsonEx.h>

USING_NS_JC;
USING_NS_JS;

SGAIInfoLoader::SGAIInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool SGAIInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Json::Value aiListRoot = root["ai"];

		for (int i = 0; i < aiListRoot.size(); ++i) {
			Value& aiRoot = aiListRoot[i];
			SGAIInfo* pInfo = dbg_new SGAIInfo;
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

void SGAIInfoLoader::readAIInfo(Json::Value& aiRoot, Out_ SGAIInfo* aiInfo) {
	aiInfo->Code = aiRoot["code"].asInt();
	SGJsonEx::parseFloatNumberN(aiRoot["wander_prob"], aiInfo->WanderProbs, AIWanderDecision::Max - 1);
	SGJsonEx::parseFloatNumberN(aiRoot["track_prob"], aiInfo->TrackProbs, AITrackDecision::Max - 1);
	SGJsonEx::parseFloatNumberN(aiRoot["angry_prob"], aiInfo->AngryProbs, AIAngryDecision::Max - 1);
	SGJsonEx::parseFloatNumberN(aiRoot["idle_time"], aiInfo->IdleTime, 2);
	SGJsonEx::parseFloatNumberN(aiRoot["wander_walk_time"], aiInfo->WanderWalkTime, 2);
	SGJsonEx::parseFloatNumberN(aiRoot["follow_walk_time"], aiInfo->FollowWalkTime, 2);
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
