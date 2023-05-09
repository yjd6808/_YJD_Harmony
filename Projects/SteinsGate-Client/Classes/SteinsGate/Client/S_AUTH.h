/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 3:00:14 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/SendHelper.h>

struct S_AUTH : JNetwork::SendHelper<S_AUTH>
{
	static void SendLogin(const std::string& id, const std::string& pw);
};


