/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:32:05 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct CommonInfoLoader : ConfigFileLoaderAbstract
{
public:
	virtual ~CommonInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Common; }
	bool load() override;

	static void readCommonInfo(Json::Value& commonRoot, Out_ CommonInfo* commonInfo);
};