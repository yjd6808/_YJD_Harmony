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


struct AccountData
{
	AccountData();

	void Clear();

	SGStaticString<Const::StringLen::AccountId> Id;
	SGStaticString<Const::StringLen::AccountPass> Pass;
	SGDateTime LastLogin;
	GameServerType_t LastServer;
};


using AuthenticationSerial_t = Int32;
struct AuthenticationData
{
	AuthenticationState_t State;
	AuthenticationSerial_t Serial;				// 토큰 데이터
	SGDateTime TimeId;							// 최신화된 시각 및 고유 시각ID
	AccountData AccountData;					// 처음 계정 접속시 DB로부터 불러왔던 계정 정보
};