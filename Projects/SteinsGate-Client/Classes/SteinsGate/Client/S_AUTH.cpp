/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 3:04:57 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "S_AUTH.h"

#include <SteinsGate/Common/Cmd_AUTH.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

void S_AUTH::SEND_CAU_Login(const char* id, const char* pw) {
	const auto sending = SendBegin<CAU_Login>();
	sending.Cmd.Id.SetStringUnsafe(id);
	sending.Cmd.Pass.SetStringUnsafe(pw);
}