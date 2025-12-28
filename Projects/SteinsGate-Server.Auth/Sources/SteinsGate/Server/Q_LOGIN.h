#pragma once

#include <JCore/Define.h>

#include <SteinsGate/Common/Struct.h>
#include <SteinsGate/Common/QueryHelper.h>

struct Q_LOGIN : Qry::QryHelper<Q_LOGIN>
{
	static void SelectAccountInfoList(const char* _pAccountId);
	static bool RegisterAccount(const char* _pAccountId, const char* _pAccountPass);
	static bool SelectAccountInfo(const char* _pAccountId, OUT AccountData& _accountData);
};
