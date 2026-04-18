/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:31 AM
 * =====================
 *
 */


#include "DescMgr_MapObject.h"

#include "sgcl/Util/JsonUtilEx.h"

USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
MapObjectInfoLoader::MapObjectInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MapObjectInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
		return false;
	try
	{
		Json::Value mapObjectListRoot = root["map_object"];
		for (size_t i = 0; i < mapObjectListRoot.size(); ++i)
		{
			Value& mapObjectRoot = mapObjectListRoot[(ArrayIndex)i];

			const int eType = mapObjectRoot["type"].asInt();
			MapObjectInfo* pInfo;

			switch (eType)
			{
			case MapObjectType::Obstacle:
				{
					MapObjectObstacleInfo* pObstacleInfo = dbg_new MapObjectObstacleInfo();
					ReadObstacleInfo(mapObjectRoot, pObstacleInfo);
					pInfo = pObstacleInfo;
					break;
				}
			case MapObjectType::Gate:
				{
					MapObjectGateInfo* pGateInfo = dbg_new MapObjectGateInfo();
					ReadGateInfo(mapObjectRoot, pGateInfo);
					pInfo = pGateInfo;
					break;
				}
			default:
				{
					jc_assert_msg(false, "올바른 맵 오브젝트 타입이 아닙니다.");
					continue;
				}
			}

			pInfo->code_ = mapObjectRoot["code"].asInt();
			pInfo->name_ = JsonUtilEx::GetString(mapObjectRoot["name"]);
			pInfo->isHittable_ = JsonUtilEx::GetBooleanDefault(mapObjectRoot["hitable"], false);
			pInfo->isColliadalble_ = JsonUtilEx::GetBooleanDefault(mapObjectRoot["collidable"], false);
			pInfo->isZOrederable_ = JsonUtilEx::GetBooleanDefault(mapObjectRoot["z_orderable"], false);
			JsonUtilEx::ParseThicknessInfo(mapObjectRoot["thickness_box"], pInfo->thicknessBox_);
			JsonUtilEx::ParseActorSpriteData(mapObjectRoot["actor_sprite_data"], &pInfo->pSpriteData_);
			AddData(pInfo);
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

//////////////////////////////////////////////////////////////////////////////////////////
void MapObjectInfoLoader::ReadObstacleInfo(Json::Value& _mapObjectRoot, MapObjectObstacleInfo* _pInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapObjectInfoLoader::ReadGateInfo(Json::Value& _mapObjectRoot, MapObjectGateInfo* _pInfo)
{
}
