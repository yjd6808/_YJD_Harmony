/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:43 AM
 * =====================
 *
 */


#pragma once

#include <sg/CharCommonInfo.h>
#include <sg/_Util/DescLoaderAbstract.h>

struct CharCommonInfoLoader : DescLoaderAbstract
{
public:
	CharCommonInfoLoader(DescLoaderMgr* _pManager);
	~CharCommonInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Char_Common; }
	bool Load() override;

	static void ReadCharCommonInfo(Json::Value& _charCommonRoot, OUT CharCommonInfo* _pCharCommonInfo);
};
