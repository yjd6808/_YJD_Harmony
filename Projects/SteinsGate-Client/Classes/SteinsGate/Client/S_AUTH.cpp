/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 3:04:57 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "S_AUTH.h"

#include <SteinsGate/Common/NetAuthCmd.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

void S_AUTH::SendLogin(const std::string& id, const std::string& pw) {
	auto sending = SendBegin<CmdLogin>();
	sending.Cmd.Id.SetStringUnsafe(id);
	sending.Cmd.Pw.SetStringUnsafe(pw);
}