/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 1:44:44 PM
 * =====================
 *
 */

#pragma once

#include <sg/DatabaseInfo.h>
#include <sg/DescLoaderAbstract.h>

struct DatabaseInfoLoader : DescLoaderAbstract
{
public:
	DatabaseInfoLoader(DescLoaderMgr* _pManager);

	bool Load() override;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Database;
	}

	static void ReadDatabaseInfo(Json::Value& _databaseRoot, DatabaseInfo* _pDatabaseInfo);
};
