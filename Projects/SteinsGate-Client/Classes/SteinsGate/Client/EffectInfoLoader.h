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
	EffectInfoLoader(DataManagerAbstract* _pManager);

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Effect;
	}

	bool Load() override;

	static void ReadEffectInfo(Json::Value& _effectRoot, JCORE_OUT EffectInfo* _pEffectInfo);
};
