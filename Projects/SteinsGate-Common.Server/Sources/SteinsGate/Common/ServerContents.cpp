/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 3:00:10 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "ServerContents.h"

#include <SteinsGate/Common/UnauthenticatedSessionManager.h>

//////////////////////////////////////////////////////////////////////////////////////////
void ServerContents::Initialize()
{
	if (Core::ServerProcessInfo->processType_ == ServerProcessType::Auth)
	{
	}
	else if (Core::ServerProcessInfo->processType_ == ServerProcessType::Lobby)
	{
		UnauthenticatedSessionManager = UnauthenticatedSessionManager::CreateInstance();
	}
	else if (Core::ServerProcessInfo->processType_ == ServerProcessType::Game)
	{
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerContents::Finalize()
{
	JCORE_DELETE_SAFE(UnauthenticatedSessionManager);
}
