/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:28 PM
 * =====================
 *
 */


#pragma once



#pragma once

#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>
#include <SteinsGate/Client/ActionInfo.h>

struct ActionInfoLoader : ConfigFileLoaderAbstract
{
public:
	ActionInfoLoader(DataManagerAbstract* manager);
	~ActionInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Action; }
	bool load() override;

	static void readActionInfo(Json::Value& actionRoot, Out_ ActionInfo* actionInfo);
};