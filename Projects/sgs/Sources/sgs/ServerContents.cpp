/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 3:00:10 PM
 * =====================
 *
 */


#include "Core.h"
#include "ServerCoreHeader.h"
#include "ServerContents.h"

#include <sgs/UnauthenticatedSessionManager.h>

//////////////////////////////////////////////////////////////////////////////////////////
void ServerContents::Initialize()
{
	if (sg::ServerProcessInfo->processType_ == ServerProcessType::Auth)
	{
	}
	else if (sg::ServerProcessInfo->processType_ == ServerProcessType::Lobby)
	{
		UnauthenticatedSessionManager = dbg_new::UnauthenticatedSessionManager();
	}
	else if (sg::ServerProcessInfo->processType_ == ServerProcessType::Game)
	{
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerContents::Finalize()
{
	JC_DELETE_SAFE(UnauthenticatedSessionManager);
}
