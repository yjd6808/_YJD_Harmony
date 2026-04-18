/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 4:43:09 PM
 * =====================
 *
 */

#pragma once

#include "sg/Struct/SteinsGate_Effect.h"
#include "sg/Util/DescLoaderAbstract.h"

struct EffectInfoLoader : DescLoaderAbstract
{
public:
	EffectInfoLoader();

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Effect;
	}

	bool Load() override;

	static void ReadEffectInfo(Json::Value& _effectRoot, OUT EffectInfo* _pEffectInfo);
};
