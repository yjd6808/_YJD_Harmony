/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:26 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/SGObstacleInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>


struct SGObstacleInfoLoader : ConfigFileLoaderAbstract
{
public:
	SGObstacleInfoLoader(DataManagerAbstract* manager);
	~SGObstacleInfoLoader() override = default;

	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Obstacle; }

	static void readObstacleInfo(Json::Value& obstacleRoot, Out_ SGObstacleInfo* obstacleInfo);
};


