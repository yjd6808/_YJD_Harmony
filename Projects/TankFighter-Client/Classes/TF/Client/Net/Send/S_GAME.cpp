/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 5:10:30 PM
 * =====================
 *
 */

#include "Pch.h"
#include "S_GAME.h"

USING_NS_JC;

void S_GAME::SEND_CS_Login(const String& id, const String& pass) {
	auto sending = SendBegin<CS_Login>();
	sending.Cmd.Id = id;
	sending.Cmd.Password = pass;
}

void S_GAME::SEND_CS_Register(const JCore::String& id, const JCore::String& pass) {
	auto sending = SendBegin<CS_Register>();
	sending.Cmd.Id = id;
	sending.Cmd.Password = pass;
}

