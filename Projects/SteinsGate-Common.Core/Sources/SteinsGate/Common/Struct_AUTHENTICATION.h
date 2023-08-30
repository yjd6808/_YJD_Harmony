/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 11:03:02 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Enum.h>
#include <SteinsGate/Common/Const.h>
#include <SteinsGate/Common/Type.h>


using AuthenticationSerial_t = Int32;
struct AuthenticationData
{
	AuthenticationState_t State;
	AuthenticationSerial_t Serial;				// 토큰 데이터
	SGStaticString<Const::StringLen::AccountId> AccountId;
	SGDateTime TimeId;							// 최신화된 시각 및 고유 시각ID
};

struct AccountData
{
	AccountData();

	void Clear();

	SGStaticString<Const::StringLen::AccountId> Id;
	SGStaticString<Const::StringLen::AccountPass> Pass;
	SGDateTime LastLogin;
	GameServerType_t LastServer;
};
