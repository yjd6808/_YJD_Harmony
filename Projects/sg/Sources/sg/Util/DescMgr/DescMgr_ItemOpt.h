/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 6:37:55 AM
 * =====================
 *
 */


#pragma once

#include "sg/Struct/SteinsGate_ItemOpt.h"
#include "sg/Util/DescLoaderAbstract.h"

struct ItemOptInfoLoader : DescLoaderAbstract
{
public:
	ItemOptInfoLoader();
	~ItemOptInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::ItemOpt;
	}

	bool Load() override;

	static void ReadItemOptInfo(Json::Value& _optRoot, OUT ItemOptInfo* _pOptInfo);
	ItemOptInfo* GetData(const jc::String& _name);

private:
	jc::HashMap<jc::String, ItemOptInfo*> dataMapByName_;
};
