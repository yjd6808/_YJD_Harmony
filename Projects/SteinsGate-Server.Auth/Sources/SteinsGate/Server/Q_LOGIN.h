#pragma once

#include <JCore/Define.h>

#include <SteinsGate/Common/Struct.h>
#include <SteinsGate/Common/QueryHelper.h>

struct Q_LOGIN : Qry::QryHelper<Q_LOGIN>
{
	static void SelectAccountInfoList();
	static bool CheckAccountExist(const char* id, const char* pass);
	static bool RegisterAccount(const char* id, const char* pass);
	static bool SelectAccountInfo(const char* id, JCORE_OUT AccountData& accountData);
};

