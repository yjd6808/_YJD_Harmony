/*
 * 작성자: 윤정도
 * 생성일: 8/7/2023 11:32:58 AM
 * =====================
 *
 */


#include "Core.h"
#include "DescMgr_MapPhysics.h"

USING_NS_JC;
USING_NS_JS;

////////////////////////////////////////////////////////////////////////////////////////////////////
MapPhysicsInfoLoader::MapPhysicsInfoLoader()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
MapPhysicsInfoLoader::~MapPhysicsInfoLoader()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool MapPhysicsInfoLoader::Load()
{
	Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Json::Value& physicsListRoot = root["physics"];

		for (ArrayIndex i = 0; i < physicsListRoot.size(); ++i)
		{
			Value& physicsRoot = physicsListRoot[i];
			MapPhysicsInfo* pMapPhysicsInfo = dbg_new MapPhysicsInfo;
			ReadMapPhysicsInfo(physicsRoot, pMapPhysicsInfo);
			AddData(pMapPhysicsInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void MapPhysicsInfoLoader::ReadMapPhysicsInfo(Value& _mapRoot, OUT MapPhysicsInfo* _pMapPhysicsInfo)
{
	_pMapPhysicsInfo->code_ = _mapRoot["code"].asInt();
	_pMapPhysicsInfo->gravity_ = _mapRoot["gravity"].asFloat();
	_pMapPhysicsInfo->friction_ = _mapRoot["friction"].asFloat();
	_pMapPhysicsInfo->elasticityDividedForce_ = _mapRoot["elasticity_divided_force"].asFloat();
}
