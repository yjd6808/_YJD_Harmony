/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 5:08:11 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/SendHelper.h>

struct S_GAME : JNetwork::SendHelper<S_GAME>
{
	static void SEND_CS_Login(const JCore::String& id, const JCore::String& pass);
	static void SEND_CS_Register(const JCore::String& id, const JCore::String& pass);
};
