/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:26 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/ObstacleInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>


struct ObstacleInfoLoader : ConfigFileLoaderAbstract
{
public:
	ObstacleInfoLoader(DataManagerAbstract* manager);
	~ObstacleInfoLoader() override = default;

	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Obstacle; }

	static void readObstacleInfo(Json::Value& obstacleRoot, JCORE_OUT ObstacleInfo* obstacleInfo);
};


