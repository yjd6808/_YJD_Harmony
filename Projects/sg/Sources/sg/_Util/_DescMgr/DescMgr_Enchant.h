/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:43 AM
 * =====================
 *
 */


#pragma once

#include <sg/EnchantInfo.h>
#include <sg/_Util/DescLoaderAbstract.h>

struct EnchantInfoLoader : DescLoaderAbstract
{
public:
	EnchantInfoLoader(DescLoaderMgr* _pManager);
	~EnchantInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Enchant;
	}

	bool Load() override;

	/* static */
	void ReadEnchantInfo(Json::Value& _enchantRoot, OUT EnchantInfo* _pEnchantInfo);
};
