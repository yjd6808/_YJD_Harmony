#pragma once

#include "jc/Define.h"

#include "jdb/QueryHelper.h"

struct Q_LOGIN : jdb::QryHelper<Q_LOGIN>
{
	static void SelectAccountInfoList(const char* _pAccountId);
	static bool RegisterAccount(const char* _pAccountId, const char* _pAccountPass);
	static bool SelectAccountInfo(const char* _pAccountId, OUT AccountData& _accountData);
};
