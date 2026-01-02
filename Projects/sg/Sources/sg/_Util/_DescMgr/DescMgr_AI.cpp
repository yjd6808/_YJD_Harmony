/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:53:06 PM
 * =====================
 *
 */

#include "Core.h"
#include "DescMgr_AI.h"

#include <sg/_Util/JsonUtil.h>

USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
AIInfoLoader::AIInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AIInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Json::Value aiListRoot = root["ai"];

		for (int i = 0; i < aiListRoot.size(); ++i)
		{
			Value& aiRoot = aiListRoot[i];
			AIInfo* pInfo = dbg_new AIInfo;
			ReadAiInfo(aiRoot, pInfo);
			AddData(pInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileType(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIInfoLoader::ReadAiInfo(Json::Value& _aiRoot, OUT AIInfo* _pAiInfo)
{
	_pAiInfo->code_ = _aiRoot["code"].asInt();
	JsonUtil::ParseFloatNumberN(_aiRoot["wander_prob"], _pAiInfo->wanderProbs_, AIWanderDecision::Max - 1);
	JsonUtil::ParseFloatNumberN(_aiRoot["track_prob"], _pAiInfo->trackProbs_, AITrackDecision::Max - 1);
	JsonUtil::ParseFloatNumberN(_aiRoot["angry_prob"], _pAiInfo->angryProbs_, AIAngryDecision::Max - 1);
	JsonUtil::ParseFloatNumberN(_aiRoot["idle_time"], _pAiInfo->idleTime_, 2);
	JsonUtil::ParseFloatNumberN(_aiRoot["wander_walk_time"], _pAiInfo->wanderWalkTime_, 2);
	JsonUtil::ParseFloatNumberN(_aiRoot["track_walk_time"], _pAiInfo->trackWalkTime_, 2);
	_pAiInfo->forceTrack_ = _aiRoot["force_track"].asBool();
	_pAiInfo->forceAngry_ = _aiRoot["force_angry"].asBool();
	_pAiInfo->sightRadious_ = _aiRoot["sight_radious"].asFloat();
	_pAiInfo->attackRadious_ = _aiRoot["attack_radious"].asFloat();

	// 확률 사용하기 쉽도록 변경
	for (int j = 1; j < AIWanderDecision::Max - 1; ++j)
	{
		_pAiInfo->wanderProbs_[j] += _pAiInfo->wanderProbs_[j - 1];
	}

	for (int j = 1; j < AITrackDecision::Max - 1; ++j)
	{
		_pAiInfo->trackProbs_[j] += _pAiInfo->trackProbs_[j - 1];
	}

	for (int j = 1; j < AIAngryDecision::Max - 1; ++j)
	{
		_pAiInfo->angryProbs_[j] += _pAiInfo->angryProbs_[j - 1];
	}
}
