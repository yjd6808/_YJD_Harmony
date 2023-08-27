/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:23:52 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "S_AUTHENTICATION.h"

#include <SteinsGate/Common/Cmd_AUTHENTICATION.h>

USING_NS_JC;
USING_NS_JNET;

void S_AUTHENTICATION::SEND_AUC_LoginAck(LoginResult_t result, GameServerType_t lastServer /* = GameServerType::Max */, AuthenticationSerial_t serial /* = InvalidValue_v */) {
	const auto sending = SendBegin<AUC_LoginAck>();
	sending.Cmd.Result = result;
	sending.Cmd.LastServer = lastServer;
	sending.Cmd.Serial = serial;
}