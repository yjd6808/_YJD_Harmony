/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:52:56 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/AIInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct AIInfoLoader : ConfigFileLoaderAbstract
{
public:
	AIInfoLoader(DataManagerAbstract* _pManager);
	~AIInfoLoader() override = default;

	bool Load() override;
	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::AI; }

	static void ReadAiInfo(Json::Value& _aiRoot, JCORE_OUT AIInfo* _pAiInfo);
};
