/*
 * 작성자: 윤정도
 * 생성일: 5/14/2023 12:02:07 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "Q_LOGIN.h"

#include <SteinsGate/Common/QueryDefine_AUTH.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void Q_LOGIN::SelectAccountInfoList(const char* _pAccountId)
{
	Qry::SelectAccountInfoListResult result;
	Qry::SelectAccountInfoList::Execute<THelper>(Core::GameDB, result);

	do
	{

	} while (result.FetchNextRow());
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Q_LOGIN::RegisterAccount(const char* _pAccountId, const char* _pAccountPass)
{
	Qry::InsertResult result;
	Qry::InsertAccountInfo::Execute<THelper>(Core::GameDB, result, _pAccountId, _pAccountPass);
	return IsSuccess;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Q_LOGIN::SelectAccountInfo(const char* _pAccountId, JCORE_OUT AccountData& _accountData)
{
	Qry::SelectAccountInfoResult result;
	Qry::SelectAccountInfo::Execute<THelper>(Core::GameDB, result, _pAccountId);

	// 쿼리는 성공했지만 바인딩된 결과물이 없으면 실패로 간주
	if (!IsSuccess || !result.HasBindedResult)
		return false;

	if (result.LastServer < GameServerType::Begin || result.LastServer > GameServerType::End)
	{
		DebugAssert(false);
		return false;
	}

	_accountData.LastLogin = DateTime::Now();
	_accountData.LastServer = (GameServerType_t)result.LastServer;
	_accountData.Id = result.Id;
	_accountData.Pass = result.Pass;
	return true;
}
