/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Core.h"
#include "DatabaseInfoLoader.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/SGJson.h>

USING_NS_JC;
USING_NS_JS;

DatabaseInfoLoader::DatabaseInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool DatabaseInfoLoader::load() {
	Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Value& databaseListRoot = root["database"];

		for (int i = 0; i < databaseListRoot.size(); ++i) {
			Value& databasetRoot = databaseListRoot[i];
			DatabaseInfo* pDatabaseInfo = dbg_new DatabaseInfo;
			readDatabaseInfo(databasetRoot, pDatabaseInfo);
			addData(pDatabaseInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void DatabaseInfoLoader::readDatabaseInfo(Value& databaseRoot, DatabaseInfo* databaseInfo) {
	databaseInfo->Code = databaseRoot["code"].asInt();
	databaseInfo->Name = SGJson::getString(databaseRoot["name"]);
	databaseInfo->HostName = SGJson::getString(databaseRoot["hostname"]);
	databaseInfo->ConnectionPort = (Int16U)databaseRoot["connection_port"].asInt();
	SGJson::parseIntNumberN(databaseRoot["use"], databaseInfo->Use, ServerProcessType::Max);
	SGJson::parseIntNumberN(databaseRoot["connection_pool_size"], databaseInfo->ConnectionPoolSize, ServerProcessType::Max);
	SGJson::parseIntNumberN(databaseRoot["max_connection"], databaseInfo->MaxConnection, ServerProcessType::Max);
	SGJson::parseIntNumberN(databaseRoot["iocp_thread_count"], databaseInfo->IocpThreadCount, ServerProcessType::Max);
	databaseInfo->AccountId = SGJson::getString(databaseRoot["account_id"]);
	databaseInfo->AccountPass = SGJson::getString(databaseRoot["account_pass"]);
	databaseInfo->SchemaName = SGJson::getString(databaseRoot["schema_name"]);
}


