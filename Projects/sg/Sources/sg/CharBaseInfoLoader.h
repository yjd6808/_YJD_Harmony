/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <sg/CharBaseInfo.h>
#include <sg/DescLoaderAbstract.h>

struct CharBaseInfoLoader : DescLoaderAbstract
{
public:
	CharBaseInfoLoader(DescLoaderMgr* _pManager);

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Char_Base; }

	bool Load() override;

	static void ReadCharBaseInfo(Json::Value& _charBaseRoot, CharBaseInfo* _pBaseInfo);
	static void ReadGunnerInfo(Json::Value& _gunnerBaseRoot, GunnerInfo* _pBaseInfo);
};
