/*
 * 작성자: 윤정도
 * 생성일: 1/1/2026 11:20:44 AM
 * =====================
 *
 */

#include "sgapiServerBase.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
bool sgapiServerBase::sgapi_IsInterServerClient()
{
	if (serverProcessType_ == ServerProcessType::Center) // 중앙 서버는 인터 서버이다.
		return false;
	return isInterServerClient_; // 그 외(인증/로비/게임) 서버가 인터서버에 접속하는 클라이언트임.
}

//////////////////////////////////////////////////////////////////////////////////////////
void sgapiServerBase::Init()
{
	sgapiBase::Init();

	String moduleName = Env::ModuleName();
	if (moduleName == "sgs_auth")
	{
		serverProcessType_ = ServerProcessType::Auth;
		isInterServerClient_ = true;
	}
	else if (moduleName == "sgs_center")
	{
		serverProcessType_ = ServerProcessType::Center;
	}
	else if (moduleName == "sgs_lobby")
	{
		serverProcessType_ = ServerProcessType::Lobby;
		isInterServerClient_ = true;
	}
	else if (moduleName == "sgs_game")
	{
		serverProcessType_ = ServerProcessType::Game;
		isInterServerClient_ = true;
	}
	else
	{
		jc_assert_msg(false, "알 수 없는 서버 프로세스 타입입니다. (%s)", moduleName.Source());
	}
}
