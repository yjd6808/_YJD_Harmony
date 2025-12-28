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
#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JC;
USING_NS_JS;

////////////////////////////////////////////////////////////////////////////////////////
DatabaseInfoLoader::DatabaseInfoLoader(DataManagerAbstract* _pManager)
: ConfigFileLoaderAbstract(_pManager)
{
}

////////////////////////////////////////////////////////////////////////////////////////
bool DatabaseInfoLoader::Load()
{
	Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Value& databaseListRoot = root["database"];

		for (int i = 0; i < databaseListRoot.size(); ++i)
		{
			Value& databaseRoot = databaseListRoot[i];
			DatabaseInfo* pDatabaseInfo = dbg_new DatabaseInfo;
			ReadDatabaseInfo(databaseRoot, pDatabaseInfo);
			AddData(pDatabaseInfo);
		}
	}
	catch (std::exception& _ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), _ex.what());
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
void DatabaseInfoLoader::ReadDatabaseInfo(Value& _databaseRoot, DatabaseInfo* _pDatabaseInfo)
{
	_pDatabaseInfo->code_ = _databaseRoot["code"].asInt();
	_pDatabaseInfo->name_ = JsonUtil::GetString(_databaseRoot["name"]);
	_pDatabaseInfo->hostName_ = JsonUtil::GetString(_databaseRoot["hostname"]);
	_pDatabaseInfo->connPort_ = (Int16U)_databaseRoot["connection_port"].asInt();

	// 처음껀 안쓰므로 인덱스 1부터 채운다.
	JsonUtil::ParseIntNumberN(_databaseRoot["use"], _pDatabaseInfo->use_ + 1, ServerProcessType::Count);
	JsonUtil::ParseIntNumberN(_databaseRoot["connection_pool_size"], _pDatabaseInfo->connPoolSize_ + 1,ServerProcessType::Count);
	JsonUtil::ParseIntNumberN(_databaseRoot["max_connection"], _pDatabaseInfo->maxConnection_ + 1, ServerProcessType::Count);
	JsonUtil::ParseIntNumberN(_databaseRoot["iocp_thread_count"], _pDatabaseInfo->iocpThreadCount_ + 1, ServerProcessType::Count);
	_pDatabaseInfo->accountId_ = JsonUtil::GetString(_databaseRoot["account_id"]);
	_pDatabaseInfo->accountPass_ = JsonUtil::GetString(_databaseRoot["account_pass"]);
	_pDatabaseInfo->schemaName_ = JsonUtil::GetString(_databaseRoot["schema_name"]);
}
