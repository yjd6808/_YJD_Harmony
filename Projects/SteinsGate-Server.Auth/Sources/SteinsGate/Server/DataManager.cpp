/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 4:03:28 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "DataManager.h"

#include <SteinsGate/Common/DatabaseInfoLoader.h>
#include <SteinsGate/Common/ServerInfoLoader.h>

void DataManager::initializeLoader() {
	m_pConfigFileLoaders[ConfigFileType::Database] = dbg_new DatabaseInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Server] = dbg_new ServerInfoLoader(this);
}