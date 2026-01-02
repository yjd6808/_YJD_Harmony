/*
 * 작성자: 윤정도
 * 생성일: 5/14/2023 12:02:07 PM
 * =====================
 *
 */

#include "Core.h"
#include "AuthCoreHeader.h"
#include "Q_LOGIN.h"

#include <sgs/QueryDefine_AUTH.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void Q_LOGIN::SelectAccountInfoList(const char* _pAccountId)
{
	Qry::SelectAccountInfoListResult result;
	Qry::SelectAccountInfoList::Execute<THelper>(sg::GameDB, result);

	do
	{

	} while (result.FetchNextRow());
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Q_LOGIN::RegisterAccount(const char* _pAccountId, const char* _pAccountPass)
{
	Qry::InsertResult result;
	Qry::InsertAccountInfo::Execute<THelper>(sg::GameDB, result, _pAccountId, _pAccountPass);
	return IsSuccess;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Q_LOGIN::SelectAccountInfo(const char* _pAccountId, OUT AccountData& _accountData)
{
	Qry::SelectAccountInfoResult result;
	Qry::SelectAccountInfo::Execute<THelper>(sg::GameDB, result, _pAccountId);

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
