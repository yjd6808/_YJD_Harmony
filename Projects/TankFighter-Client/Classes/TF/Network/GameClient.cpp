#define _WINSOCKAPI_

#include <TF/Network/GameClient.h>


GameClient::~GameClient() {
	DeleteSafe(m_EventListener);
	DeleteSafe(m_Parser);
}

GameClient* GameClient::GetInstance() {
	if (ms_pInstance == nullptr) {
		ms_pInstance = new GameClient();
		ms_pInstance->m_Parser = new CommandParser();
		ms_pInstance->m_EventListener = new GameClientEventListener();
		ms_pInstance->InitializeParser();
		ms_pInstance->SetEventListener(ms_pInstance->m_EventListener);
	}

	return ms_pInstance;
}

void GameClient::InitializeParser() {
	// 쓸일이 없다.
}

void GameClient::UpdateCharacterInfo(CharacterInfo& info) {
	Memory::CopyUnsafe(&m_CharacterInfo, &info, sizeof(info));
}


