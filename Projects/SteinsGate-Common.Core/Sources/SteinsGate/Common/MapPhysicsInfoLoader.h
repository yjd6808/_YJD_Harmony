/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:33:11 AM
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




