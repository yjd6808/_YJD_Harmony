#include "Core.h"
#include "../Structure.h"

#include "MysqlStatementBuilder.h"
#include "MysqlDatabase.h"

NS_JDB_BEGIN

MysqlConnection* MysqlStatementBuilder::ms_pConn = nullptr;
jc::AtomicInt MysqlStatementBuilder::ms_refCount = 0;

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlStatementBuilder::Initialize(const DatabaseInfo& _dbInfo)
{
	if (ms_pConn != nullptr)
	{
		++ms_refCount;
		return true;
	}

	// mysql_real_escape_string 이 함수 사용하기위해서 어쩔수없이 커넥션 사용하도록 함
	// 클라이언트 라이브러리 함수라서 실제로 이스케이프 함수 실행시마다 DB에 접속하지는 않는다.
	// 단지 클라이언트 라이브러리 초기화 여부를 확인하는 용도로 쓰임.

	++ms_refCount;
	ms_pConn = dbg_new MysqlConnection();
	return ms_pConn->Connect(
		_dbInfo.hostName_,
		_dbInfo.connPort_,
		_dbInfo.accountId_,
		_dbInfo.accountPass_,
		_dbInfo.dbName_
	);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MysqlStatementBuilder::Finalize()
{
	if (ms_pConn && (--ms_refCount) <= 0)
	{
		ms_pConn->Disconnect();
		JC_DELETE_SAFE(ms_pConn);
	}
}

NS_END