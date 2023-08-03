/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "DataManager.h"

#include <SteinsGate/Common/DatabaseInfoLoader.h>
#include <SteinsGate/Common/ServerInfoLoader.h>

DataManager::DataManager() {
	DataManager::initializeLoader();
}

void DataManager::initializeLoader() {
	m_pConfigFileLoaders[ConfigFileType::Database] = dbg_new DatabaseInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Server] = dbg_new ServerInfoLoader(this);
}

