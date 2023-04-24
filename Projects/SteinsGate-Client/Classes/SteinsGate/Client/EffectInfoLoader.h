/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 4:43:09 PM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/EffectInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct EffectInfoLoader : ConfigFileLoaderAbstract
{
public:
	EffectInfoLoader(DataManagerAbstract* manager);
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Effect; }
	bool load() override;

	static void readEffectInfo(Json::Value& effectRoot, Out_ EffectInfo* effectInfo);
};


