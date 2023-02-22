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
	EnchantInfoLoader(DataManagerAbstract* manager);
	virtual ~EnchantInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Enchant; }
	bool load() override;

	/* static */ void readEnchantInfo(Json::Value & enchantRoot, Out_ EnchantInfo * enchantInfo);
};
