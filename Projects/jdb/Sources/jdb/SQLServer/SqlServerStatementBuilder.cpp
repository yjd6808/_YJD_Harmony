#include "Core.h"
#include "../Structure.h"

#include "SqlServerStatementBuilder.h"
#include "SqlServerDatabase.h"

NS_JDB_BEGIN

bool SqlServerStatementBuilder::ms_initialized = false;

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerStatementBuilder::Initialize(const DatabaseInfo& _dbInfo)
{
	// SQL Server의 경우 MySQL과 달리 이스케이프를 위한 별도의 커넥션이 필요 없다.
	// 싱글 쿼트 더블링 방식이므로 순수 문자열 처리만으로 충분하다.
	ms_initialized = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerStatementBuilder::Finalize()
{
	ms_initialized = false;
}

NS_END
