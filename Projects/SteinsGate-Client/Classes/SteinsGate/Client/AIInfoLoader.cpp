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

//////////////////////////////////////////////////////////////////////////////////////////
AIInfoLoader::AIInfoLoader(DataManagerAbstract* _pManager)
	: ConfigFileLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AIInfoLoader::load()
{
	Json::Value root;

	if (!loadJson(root))
		return false;

	try
	{
		Json::Value aiListRoot = root["ai"];

		for (int i = 0; i < aiListRoot.size(); ++i)
		{
			Value& aiRoot = aiListRoot[i];
			AIInfo* pAiInfo = dbg_new AIInfo;
			readAIInfo(aiRoot, pAiInfo);
			addData(pAiInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileType(), ex.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIInfoLoader::readAIInfo(Json::Value& _aiRoot, JCORE_OUT AIInfo* _pAiInfo)
{
	_pAiInfo->Code = _aiRoot["code"].asInt();
	JsonUtilEx::parseFloatNumberN(_aiRoot["wander_prob"], _pAiInfo->WanderProbs, AIWanderDecision::Max - 1);
	JsonUtilEx::parseFloatNumberN(_aiRoot["track_prob"], _pAiInfo->TrackProbs, AITrackDecision::Max - 1);
	JsonUtilEx::parseFloatNumberN(_aiRoot["angry_prob"], _pAiInfo->AngryProbs, AIAngryDecision::Max - 1);
	JsonUtilEx::parseFloatNumberN(_aiRoot["idle_time"], _pAiInfo->IdleTime, 2);
	JsonUtilEx::parseFloatNumberN(_aiRoot["wander_walk_time"], _pAiInfo->WanderWalkTime, 2);
	JsonUtilEx::parseFloatNumberN(_aiRoot["track_walk_time"], _pAiInfo->TrackWalkTime, 2);
	_pAiInfo->ForceTrack = _aiRoot["force_track"].asBool();
	_pAiInfo->ForceAngry = _aiRoot["force_angry"].asBool();
	_pAiInfo->SightRadious = _aiRoot["sight_radious"].asFloat();
	_pAiInfo->AttackRadious = _aiRoot["attack_radious"].asFloat();

	// 확률 사용하기 쉽도록 변경
	for (int j = 1; j < AIWanderDecision::Max - 1; ++j)
	{
		_pAiInfo->WanderProbs[j] += _pAiInfo->WanderProbs[j - 1];
	}

	for (int j = 1; j < AITrackDecision::Max - 1; ++j)
	{
		_pAiInfo->TrackProbs[j] += _pAiInfo->TrackProbs[j - 1];
	}

	for (int j = 1; j < AIAngryDecision::Max - 1; ++j)
	{
		_pAiInfo->AngryProbs[j] += _pAiInfo->AngryProbs[j - 1];
	}
}
