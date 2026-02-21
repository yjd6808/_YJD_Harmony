/*
 * 작성자: 윤정도
 * 생성일: 5/14/2023 12:02:07 PM
 * =====================
 *
 */

#include "Core.h"
#include "Q_LOGIN.h"

#include <sgs/QueryDefine_AUTH.h>

USING_NS_JC;
USING_NS_JNET;
USING_NS_JDB;

//////////////////////////////////////////////////////////////////////////////////////////
void Q_LOGIN::SelectAccountInfoList(const char* _pAccountId)
{
	MysqlDatabase* pDatabase = nullptr;
	SelectAccountInfoListResult result;
	SelectAccountInfoList::Execute<THelper>(pDatabase, result);

	do
	{

	} while (result.FetchNextRow());
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Q_LOGIN::RegisterAccount(const char* _pAccountId, const char* _pAccountPass)
{
	MysqlDatabase* pDatabase = nullptr;
	InsertResult result;
	InsertAccountInfo::Execute<THelper>(pDatabase, result, _pAccountId, _pAccountPass);
	return IsSuccess;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Q_LOGIN::SelectAccountInfo(const char* _pAccountId, OUT AccountData& _accountData)
{
	MysqlDatabase* pDatabase = nullptr;
	SelectAccountInfoResult result;
	SelectAccountInfo::Execute<THelper>(pDatabase, result, _pAccountId);

	// 쿼리는 성공했지만 바인딩된 결과물이 없으면 실패로 간주
	if (!IsSuccess || !result.HasBindedResult)
		return false;

	if (result.LastServer < GameServerType::Begin || 
		result.LastServer > GameServerType::End) 
	{
		jc_assert(false);
		return false;
	}

	_accountData.lastLoginTime_ = DateTime::Now();
	_accountData.lastServer_ = (GameServerType_t)result.LastServer;
	_accountData.id_ = result.Id;
	_accountData.pass_ = result.Pass;
	return true;
}
