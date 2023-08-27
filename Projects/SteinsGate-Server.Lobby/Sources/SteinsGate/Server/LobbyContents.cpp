/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 1:52:29 PM
 * =====================
 *
 */


#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "LobbyContents.h"

USING_NS_JC;

void LobbyContents::Initialize() {
	ServerContents::Initialize();
}

void LobbyContents::Finalize() {
	ClearSchedule();

	ServerContents::Finalize();
}
