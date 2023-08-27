/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:23:52 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "S_AUTHENTICATION_IS.h"

#include <SteinsGate/Common/CmdRelay_AUTHENTICATION.h>

void S_AUTHENTICATION_IS::SEND_SAU_AuthenticationCheckAck(int sessionHandle, AuthenticationSerial_t serial, AuthenticationState_t state, bool success) {
	auto sending = SendBegin<SAU_AuthenticationCheckAck>();
	sending.Cmd.Success = success;
	sending.Cmd.SessionHandle = sessionHandle;
}