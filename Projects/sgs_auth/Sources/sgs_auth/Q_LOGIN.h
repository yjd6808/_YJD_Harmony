#pragma once

#include "jc/Define.h"

#include "jdb/QueryHelper.h"

struct Q_LOGIN : jdb::QryHelper<Q_LOGIN>
{
	static void SelectAccountInfoList(const _char* _pAccountId);
	static bool RegisterAccount(const _char* _pAccountId, const _char* _pAccountPass);
	static bool SelectAccountInfo(const _char* _pAccountId, OUT AccountData& _accountData);
};
