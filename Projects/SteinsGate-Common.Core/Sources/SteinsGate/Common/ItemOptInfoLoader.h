/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 6:37:55 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ItemOptInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct ItemOptInfoLoader : ConfigFileLoaderAbstract
{
public:
	ItemOptInfoLoader(DataManagerAbstract* _pManager);
	~ItemOptInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::ItemOpt;
	}

	bool Load() override;

	static void ReadItemOptInfo(Json::Value& _optRoot, JCORE_OUT ItemOptInfo* _pOptInfo);
	ItemOptInfo* GetData(const SGString& _name);

private:
	SGHashMap<SGString, ItemOptInfo*> dataMapByName_;
};
