/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:43 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/EnchantInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct EnchantInfoLoader : ConfigFileLoaderAbstract
{
public:
	EnchantInfoLoader(DataManagerAbstract* _pManager);
	~EnchantInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Enchant;
	}

	bool Load() override;

	/* static */
	void ReadEnchantInfo(Json::Value& _enchantRoot, JCORE_OUT EnchantInfo* _pEnchantInfo);
};
