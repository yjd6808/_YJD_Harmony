/*
 * 작성자: 윤정도
 * 생성일: 5/14/2023 12:02:07 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "Q_LOGIN.h"

#include <SteinsGate/Common/QueryDefine_LOGIN.h>

USING_NS_JC;
USING_NS_JNET;

void Q_LOGIN::SelectAccountInfoList() {
	Qry::SelectAccountInfoResult result;
	Qry::SelectAccountInfo::Execute<THelper>(CoreGameDB_v, result);

	do {

	} while (result.FetchNextRow());
}

bool Q_LOGIN::CheckAccountExist(const char* id, const char* pass) {
	Qry::SelectAccountInfoResult result;
	Qry::SelectAccountInfo::Execute<THelper>(CoreGameDB_v, result, id, pass);
	return result.HasNext();
}

bool Q_LOGIN::RegisterAccount(const char* id, const char* pass) {
	Qry::InsertResult result;
	Qry::InsertAccountInfo::Execute<THelper>(CoreGameDB_v, result, id, pass);
	return IsSuccess;
}

bool Q_LOGIN::SelectAccountInfo(const char* id, const char* pass, JCORE_OUT AccountData& accountData) {
	Qry::SelectAccountInfoResult result;
	Qry::SelectAccountInfo::Execute<THelper>(CoreGameDB_v, result, id, pass);

	if (!result.HasNext())
		return false;

	accountData.LastLogin = DateTime::Now();
	accountData.LastServer = result.LastServer;
	accountData.DBTableId = result.AccountId;
	accountData.Id = result.Id;
	accountData.Pass = result.Pass;
	return true;
}
