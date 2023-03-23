/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:01:29 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGDataManager.h"

#include <SteinsGate/Common/ItemOptInfoLoader.h>
#include <SteinsGate/Common/ChannelInfoLoader.h>
#include <SteinsGate/Common/EnchantInfoLoader.h>
#include <SteinsGate/Common/ServerInfoLoader.h>
#include <SteinsGate/Common/ItemInfoLoader.h>
#include <SteinsGate/Client/SGActionInfoLoader.h>
#include <SteinsGate/Client/SGMobInfoLoader.h>
#include <SteinsGate/Client/SGCharInfoLoader.h>
#include <SteinsGate/Client/SGProjectileInfoLoader.h>
#include <SteinsGate/Client/SGClientInfoLoader.h>
#include <SteinsGate/Client/SGTileInfoLoader.h>
#include <SteinsGate/Client/SGObstacleInfoLoader.h>
#include <SteinsGate/Client/SGMapInfoLoader.h>
#include <SteinsGate/Client/SGAIInfoLoader.h>
#include <SteinsGate/Client/SGAttackDataInfoLoader.h>
#include <SteinsGate/Client/SGCharAnimationInfoLoader.h>
#include <SteinsGate/Client/SGEffectInfoLoader.h>
#include <SteinsGate/Client/SGUIInfoLoader.h>




SGDataManager::SGDataManager()
	: DataManagerAbstract()
{}

void SGDataManager::initializeLoader() {

	// Effect
	// Map
	// Monster
	// Obstacle
	// Projectile
	// Server
	// Tile
	// UI
	// Action
	// AI
	// AttackBox
	// AttackData
	// Channel
	// Char_Animation
	// Char_Base
	// Client
	// Item
	// ItemOpt
	// Enchant
	// Common

	 m_pConfigFileLoaders[ConfigFileType::Effect]			 = dbg_new SGEffectInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Map]				 = dbg_new SGMapInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Monster]			 = dbg_new SGMobInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Obstacle]			 = dbg_new SGObstacleInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Projectile]		 = dbg_new SGProjectileInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Server]			 = dbg_new ServerInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Tile]				 = dbg_new SGTileInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::UI]				 = dbg_new SGUIInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Action]			 = dbg_new SGActionInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::AI]				 = dbg_new SGAIInfoLoader(this);
	 // m_pConfigFileLoaders[ConfigFileType::AttackBox]
	 m_pConfigFileLoaders[ConfigFileType::AttackData]		 = dbg_new SGAttackDataInfoLoader(this);
	 // m_pConfigFileLoaders[ConfigFileType::Channel]			 = dbg_new SGChannel(this);
	 m_pConfigFileLoaders[ConfigFileType::Char_Animation]	 = dbg_new SGCharAnimationInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Char_Base]	     = dbg_new SGCharInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Client]			 = dbg_new SGClientInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Item]				 = dbg_new ItemInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::ItemOpt]			 = dbg_new ItemOptInfoLoader(this);
	// m_pConfigFileLoaders[ConfigFileType::Enchant]			 = dbg_new EnchantInfoLoader(this);
	 m_bInitialized = true;
}

SGMobInfo* SGDataManager::getMonsterInfo(int mobCode) {

	auto eType = ConfigFileType::Monster;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGMobInfo*)getData(eType, mobCode);
}

SGActionInfo* SGDataManager::getActionInfo(int actionCode) {
	auto eType = ConfigFileType::Action;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGActionInfo*)getData(eType, actionCode);
}


SGProjectileInfo* SGDataManager::getProjectileInfo(int projectileCode) {
	auto eType = ConfigFileType::Projectile;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGProjectileInfo*)getData(eType, projectileCode);
}

SGCharInfo* SGDataManager::getCharInfo(int charCode) {
	auto eType = ConfigFileType::Char_Base;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGCharInfo*)getData(eType, charCode);
}

SGAnimationInfo* SGDataManager::getCharAnimationInfo(int charAnimationCode) {
	auto eType = ConfigFileType::Char_Animation;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGAnimationInfo*)getData(eType, charAnimationCode);
}

SGVector<SGAnimationInfo*>& SGDataManager::getCharAnimationInfoList(int charCode) {
	auto eType = ConfigFileType::Char_Animation;

	if (!m_bLoaded[eType])
		load(eType);

	auto pLoader = (SGCharAnimationInfoLoader*)m_pConfigFileLoaders[eType];
	return pLoader->getAnimationList((CharType_t)charCode);
}

SGClientInfo* SGDataManager::getClientInfo(int code) {
	auto eType = ConfigFileType::Client;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGClientInfo*)getData(eType, code);
}


SGTileInfo* SGDataManager::getTileInfo(int tileCode) {
	auto eType = ConfigFileType::Tile;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGTileInfo*)getData(eType, tileCode);
}

SGObstacleInfo* SGDataManager::getObstacleInfo(int obstacleCode) {
	auto eType = ConfigFileType::Obstacle;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGObstacleInfo*)getData(eType, obstacleCode);
}

SGMapInfo* SGDataManager::getMapInfo(int mapCode) {
	auto eType = ConfigFileType::Map;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGMapInfo*)getData(eType, mapCode);
}

SGAIInfo* SGDataManager::getAIInfo(int aiCode) {
	auto eType = ConfigFileType::AI;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGAIInfo*)getData(eType, aiCode);
}

SGAttackDataInfo* SGDataManager::getAttackDataInfo(int attackDataCode) {

	auto eType = ConfigFileType::AttackData;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGAttackDataInfo*)getData(eType, attackDataCode);

}

SGEffectInfo* SGDataManager::getEffectInfo(int effectCode) {

	auto eType = ConfigFileType::Effect;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGEffectInfo*)getData(eType, effectCode);
}

SGUIElementInfo* SGDataManager::getUIElementInfo(int uiElementCode) {

	auto eType = ConfigFileType::UI;

	if (!m_bLoaded[eType])
		load(eType);

	return (SGUIElementInfo*)getData(eType, uiElementCode);

}

