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
	CharCommonInfoLoader(DataManagerAbstract* manager);
	virtual ~CharCommonInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Char_Common; }
	bool load() override;

	static void readCharCommonInfo(Json::Value & charCommonRoot, JCORE_OUT CharCommonInfo * charCommonInfo);
};
