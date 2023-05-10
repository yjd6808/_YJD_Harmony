/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 3:04:57 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "S_AUTH.h"

#include <SteinsGate/Common/AuthCmd.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

void S_AUTH::SendLogin(const char* id, const char* pw) {
	const auto sending = SendBegin<CmdLogin>();
	sending.Cmd.Id.SetStringUnsafe(id);
	sending.Cmd.Pw.SetStringUnsafe(pw);
}