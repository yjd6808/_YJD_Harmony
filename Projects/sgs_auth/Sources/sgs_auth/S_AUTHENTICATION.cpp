/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:23:52 PM
 * =====================
 *
 */

#include "Core.h"
#include "AuthCoreHeader.h"
#include "S_AUTHENTICATION.h"

#include <sg/Cmd_AUTHENTICATION.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void S_AUTHENTICATION::SEND_AUC_LoginAck(LoginResult_t _result, GameServerType_t _lastServer /* = GameServerType::Max */, AuthenticationSerial_t _serial /* = InvalidValue_v */)
{
	const auto sending = SendBegin<AUC_LoginAck>();
	sending.Cmd.result_ = _result;
	sending.Cmd.lastServer_ = _lastServer;
	sending.Cmd.serial_ = _serial;
}
