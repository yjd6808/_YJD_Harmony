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
	CommonInfoLoader(DataManagerAbstract* _pManager);
	~CommonInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Common; }
	bool Load() override;

	static void ReadCommonInfo(Json::Value& _commonRoot, JCORE_OUT CommonInfo* _pCommonInfo);
};
