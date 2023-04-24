/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:01:29 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "DataManager.h"

#include <SteinsGate/Common/ItemOptInfoLoader.h>
#include <SteinsGate/Common/ChannelInfoLoader.h>
#include <SteinsGate/Common/EnchantInfoLoader.h>
#include <SteinsGate/Common/CharCommonInfoLoader.h>
#include <SteinsGate/Common/ServerInfoLoader.h>
#include <SteinsGate/Common/ItemInfoLoader.h>
#include <SteinsGate/Client/ActionInfoLoader.h>
#include <SteinsGate/Client/MobInfoLoader.h>
#include <SteinsGate/Client/CharInfoLoader.h>
#include <SteinsGate/Client/ProjectileInfoLoader.h>
#include <SteinsGate/Client/ClientInfoLoader.h>
#include <SteinsGate/Client/TileInfoLoader.h>
#include <SteinsGate/Client/ObstacleInfoLoader.h>
#include <SteinsGate/Client/MapInfoLoader.h>
#include <SteinsGate/Client/AIInfoLoader.h>
#include <SteinsGate/Client/AttackDataInfoLoader.h>
#include <SteinsGate/Client/CharAnimationInfoLoader.h>
#include <SteinsGate/Client/EffectInfoLoader.h>
#include <SteinsGate/Client/UIInfoLoader.h>




DataManager::DataManager()
	: DataManagerAbstract()
{}

void DataManager::initializeLoader() {

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
	// Database

	 m_pConfigFileLoaders[ConfigFileType::Effect]			 = dbg_new EffectInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Map]				 = dbg_new MapInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Monster]			 = dbg_new MobInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Obstacle]			 = dbg_new ObstacleInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Projectile]		 = dbg_new ProjectileInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Server]			 = dbg_new ServerInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Tile]				 = dbg_new TileInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::UI]				 = dbg_new UIInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Action]			 = dbg_new ActionInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::AI]				 = dbg_new AIInfoLoader(this);
	 // m_pConfigFileLoaders[ConfigFileType::AttackBox]
	 m_pConfigFileLoaders[ConfigFileType::AttackData]		 = dbg_new AttackDataInfoLoader(this);
	 // m_pConfigFileLoaders[ConfigFileType::Channel]		 = dbg_new SGChannel(this);
	 m_pConfigFileLoaders[ConfigFileType::Char_Animation]	 = dbg_new CharAnimationInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Char_Base]	     = dbg_new CharInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Client]			 = dbg_new ClientInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Item]				 = dbg_new ItemInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::ItemOpt]			 = dbg_new ItemOptInfoLoader(this);
	 m_pConfigFileLoaders[ConfigFileType::Char_Common]		 = dbg_new CharCommonInfoLoader(this);
	 // m_pConfigFileLoaders[ConfigFileType::Enchant]		 = dbg_new EnchantInfoLoader(this);
	 m_bInitialized = true;
}

MobInfo* DataManager::getMonsterInfo(int mobCode) {

	auto eType = ConfigFileType::Monster;

	if (!m_bLoaded[eType])
		load(eType);

	return (MobInfo*)getData(eType, mobCode);
}

ActionInfo* DataManager::getActionInfo(int actionCode) {
	auto eType = ConfigFileType::Action;

	if (!m_bLoaded[eType])
		load(eType);

	return (ActionInfo*)getData(eType, actionCode);
}


ProjectileInfo* DataManager::getProjectileInfo(int projectileCode) {
	auto eType = ConfigFileType::Projectile;

	if (!m_bLoaded[eType])
		load(eType);

	return (ProjectileInfo*)getData(eType, projectileCode);
}

CharInfo* DataManager::getCharInfo(int charCode) {
	auto eType = ConfigFileType::Char_Base;

	if (!m_bLoaded[eType])
		load(eType);

	return (CharInfo*)getData(eType, charCode);
}

AnimationInfo* DataManager::getCharAnimationInfo(int charAnimationCode) {
	auto eType = ConfigFileType::Char_Animation;

	if (!m_bLoaded[eType])
		load(eType);

	return (AnimationInfo*)getData(eType, charAnimationCode);
}

SGVector<AnimationInfo*>& DataManager::getCharAnimationInfoList(int charCode) {
	auto eType = ConfigFileType::Char_Animation;

	if (!m_bLoaded[eType])
		load(eType);

	auto pLoader = (CharAnimationInfoLoader*)m_pConfigFileLoaders[eType];
	return pLoader->getAnimationList((CharType_t)charCode);
}

ClientInfo* DataManager::getClientInfo(int code) {
	auto eType = ConfigFileType::Client;

	if (!m_bLoaded[eType])
		load(eType);

	return (ClientInfo*)getData(eType, code);
}


TileInfo* DataManager::getTileInfo(int tileCode) {
	auto eType = ConfigFileType::Tile;

	if (!m_bLoaded[eType])
		load(eType);

	return (TileInfo*)getData(eType, tileCode);
}

ObstacleInfo* DataManager::getObstacleInfo(int obstacleCode) {
	auto eType = ConfigFileType::Obstacle;

	if (!m_bLoaded[eType])
		load(eType);

	return (ObstacleInfo*)getData(eType, obstacleCode);
}

MapInfo* DataManager::getMapInfo(int mapCode) {
	auto eType = ConfigFileType::Map;

	if (!m_bLoaded[eType])
		load(eType);

	return (MapInfo*)getData(eType, mapCode);
}

AIInfo* DataManager::getAIInfo(int aiCode) {
	auto eType = ConfigFileType::AI;

	if (!m_bLoaded[eType])
		load(eType);

	return (AIInfo*)getData(eType, aiCode);
}

AttackDataInfo* DataManager::getAttackDataInfo(int attackDataCode) {

	auto eType = ConfigFileType::AttackData;

	if (!m_bLoaded[eType])
		load(eType);

	return (AttackDataInfo*)getData(eType, attackDataCode);

}

EffectInfo* DataManager::getEffectInfo(int effectCode) {

	auto eType = ConfigFileType::Effect;

	if (!m_bLoaded[eType])
		load(eType);

	return (EffectInfo*)getData(eType, effectCode);
}

UIElementInfo* DataManager::getUIElementInfo(int uiElementCode) {

	auto eType = ConfigFileType::UI;

	if (!m_bLoaded[eType])
		load(eType);

	return (UIElementInfo*)getData(eType, uiElementCode);

}

