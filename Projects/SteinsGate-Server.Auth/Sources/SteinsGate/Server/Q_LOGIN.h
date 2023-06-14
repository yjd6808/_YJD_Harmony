#pragma once

#include <JCore/Define.h>
#include <SteinsGate/Common/Struct.h>

struct Q_LOGIN
{
	static void SelectAccountInfoList();
	static bool CheckAccountExist(const char* id, const char* pass);
	static bool RegisterAccount(const char* id, const char* pass);
	static bool SelectAccountInfo(const char* id, const char* pass, JCORE_OUT AccountData& accountData);
};

