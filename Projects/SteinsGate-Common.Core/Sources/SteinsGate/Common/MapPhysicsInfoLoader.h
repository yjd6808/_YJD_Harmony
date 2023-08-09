/*
 * 작성자: 윤정도
 * 생성일: 8/7/2023 11:32:58 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Common/MapPhysicsInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct MapPhysicsInfoLoader : ConfigFileLoaderAbstract
{
public:
	MapPhysicsInfoLoader(DataManagerAbstract* manager);
	~MapPhysicsInfoLoader() override;

	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::MapPhysics; }
	static void readMapPhysicsInfo(Json::Value& mapRoot, JCORE_OUT MapPhysicsInfo* mapPhysicsInfo);
};




