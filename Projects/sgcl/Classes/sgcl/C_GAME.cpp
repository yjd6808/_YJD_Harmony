/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:20:39 AM
 * =====================
 *
 */


#include "Core.h"
#include "GameCoreHeader.h"
#include "C_GAME.h"

#include <sgcl/S_GAME.h>
#include <sgcl/S_GAME_UDP.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;


//////////////////////////////////////////////////////////////////////////////////////////
void C_GAME::OnConnected(Session* _pSession)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void C_GAME::OnConnectFailed(Session* _pSession, _u32 _errorCode)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void C_GAME::OnDisconnected(Session* _pSession)
{
}
