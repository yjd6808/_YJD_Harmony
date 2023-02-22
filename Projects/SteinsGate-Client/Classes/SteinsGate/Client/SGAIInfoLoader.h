/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:52:56 PM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/SGAIInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct SGAIInfoLoader : ConfigFileLoaderAbstract
{
public:
	SGAIInfoLoader(DataManagerAbstract* manager);
	~SGAIInfoLoader() override = default;

	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::AI; }

	static void readAIInfo(Json::Value& aiRoot, Out_ SGAIInfo* aiInfo);
};


