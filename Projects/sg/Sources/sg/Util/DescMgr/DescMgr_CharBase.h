/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include "sg/Struct/SteinsGate_CharBase.h"
#include "sg/Util/DescLoaderAbstract.h"

struct CharBaseInfoLoader : DescLoaderAbstract
{
public:
	CharBaseInfoLoader();

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Char_Base; }

	bool Load() override;

	static void ReadCharBaseInfo(Json::Value& _charBaseRoot, CharBaseInfo* _pBaseInfo);
	static void ReadGunnerInfo(Json::Value& _gunnerBaseRoot, GunnerInfo* _pBaseInfo);
};
