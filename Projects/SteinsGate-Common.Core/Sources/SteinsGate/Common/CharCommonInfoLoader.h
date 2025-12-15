/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:43 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CharCommonInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct CharCommonInfoLoader : ConfigFileLoaderAbstract
{
public:
	CharCommonInfoLoader(DataManagerAbstract* _pManager);
	~CharCommonInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Char_Common; }
	bool Load() override;

	static void ReadCharCommonInfo(Json::Value& _charCommonRoot, JCORE_OUT CharCommonInfo* _pCharCommonInfo);
};
