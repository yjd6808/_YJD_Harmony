/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:52:56 PM
 * =====================
 *
 */

#pragma once

#include "sg/_Struct/SteinsGate_AI.h"
#include "sg/_Util/DescLoaderAbstract.h"

struct AIInfoLoader : DescLoaderAbstract
{
public:
	AIInfoLoader();
	~AIInfoLoader() override = default;

	bool Load() override;
	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::AI; }

	static void ReadAiInfo(Json::Value& _aiRoot, OUT AIInfo* _pAiInfo);
};
