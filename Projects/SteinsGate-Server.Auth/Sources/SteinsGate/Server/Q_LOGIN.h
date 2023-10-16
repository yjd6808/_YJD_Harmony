#pragma once

#include <JCore/Define.h>

#include <SteinsGate/Common/Struct.h>
#include <SteinsGate/Common/QueryHelper.h>

struct Q_LOGIN : Qry::QryHelper<Q_LOGIN>
{
	static void SelectAccountInfoList(const char* accountId);
	static bool RegisterAccount(const char* accountId, const char* accountPass);
	static bool SelectAccountInfo(const char* accountId, JCORE_OUT AccountData& accountData);
};

