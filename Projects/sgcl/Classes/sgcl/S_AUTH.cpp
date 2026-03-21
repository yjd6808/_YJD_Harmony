/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 3:04:57 PM
 * =====================
 *
 */


#include "S_AUTH.h"

#include "sg/Cmd_AUTHENTICATION.h"
#include "sgcl/AuthenticationComponent.h"
#include "sgcl/_Net/NetCore.h"

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void S_AUTH::SEND_CAU_Login()
{
	const auto sending = SendBegin<CAU_Login>();
	const AccountData& accountData = g_cNet.authentication_.GetAccountData();
	sending.Cmd.id_.SetString(accountData.id_);
	sending.Cmd.pass_.SetString(accountData.pass_);
}
