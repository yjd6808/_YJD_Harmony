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

	SGStaticString<Const::StringLen::AccountId> id_;
	SGStaticString<Const::StringLen::AccountPass> pass_;
	SGDateTime lastLoginTime_;
	GameServerType_t lastServer_;
};


using AuthenticationSerial_t = Int32;

struct AuthenticationData
{
	AuthenticationState_t state_;
	AuthenticationSerial_t serial_; // 토큰 데이터
	SGDateTime timeId_; // 최신화된 시각 및 고유 시각ID
	AccountData accountData_; // 처음 계정 접속시 DB로부터 불러왔던 계정 정보
};
