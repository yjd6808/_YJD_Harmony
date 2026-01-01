/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 11:03:02 AM
 * =====================
 *
 */


#pragma once

struct AccountData
{
	AccountData();

	void Clear();

	jc::StaticString<Const::StringLen::AccountId> id_;
	jc::StaticString<Const::StringLen::AccountPass> pass_;
	jc::DateTime lastLoginTime_;
	GameServerType_t lastServer_;
};


using AuthenticationSerial_t = Int32;

struct AuthenticationData
{
	AuthenticationState_t state_;
	AuthenticationSerial_t serial_; // 토큰 데이터
	jc::DateTime timeId_; // 최신화된 시각 및 고유 시각ID
	AccountData accountData_; // 처음 계정 접속시 DB로부터 불러왔던 계정 정보
};
