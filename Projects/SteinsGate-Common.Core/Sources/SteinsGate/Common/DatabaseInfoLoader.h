/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/DatabaseInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct DatabaseInfoLoader : ConfigFileLoaderAbstract
{
public:
	DatabaseInfoLoader(DataManagerAbstract* manager);

	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Database; }
	static void readDatabaseInfo(Json::Value& databaseRoot, DatabaseInfo* databaseInfo);
};

