/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 6:37:55 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ItemOptInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct ItemOptInfoLoader : ConfigFileLoaderAbstract
{
public:
	virtual ~ItemOptInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::ItemOpt; }
	bool load() override;

	static void readItemOptInfo(Json::Value& optRoot, Out_ ItemOptInfo* optInfo);
};
