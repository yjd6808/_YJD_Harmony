/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:01:29 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "DataManager.h"

#include <SteinsGate/Common/ItemOptInfoLoader.h>
#include <SteinsGate/Common/ChannelBaseInfoLoader.h>
#include <SteinsGate/Common/EnchantInfoLoader.h>
#include <SteinsGate/Common/CharCommonInfoLoader.h>
#include <SteinsGate/Common/ServerInfoLoader.h>
#include <SteinsGate/Common/ItemInfoLoader.h>
#include <SteinsGate/Common/MapInfoLoader.h>
#include <SteinsGate/Common/MapPhysicsInfoLoader.h>

#include <SteinsGate/Client/ActionInfoLoader.h>
#include <SteinsGate/Client/MonsterInfoLoader.h>
#include <SteinsGate/Client/CharInfoLoader.h>
#include <SteinsGate/Client/ProjectileInfoLoader.h>
#include <SteinsGate/Client/ClientInfoLoader.h>
#include <SteinsGate/Client/TileInfoLoader.h>
#include <SteinsGate/Client/MapObjectInfoLoader.h>
#include <SteinsGate/Client/AIInfoLoader.h>
#include <SteinsGate/Client/AttackDataInfoLoader.h>
#include <SteinsGate/Client/CharAnimationInfoLoader.h>
#include <SteinsGate/Client/EffectInfoLoader.h>
#include <SteinsGate/Client/UIInfoLoader.h>
#include <SteinsGate/Client/FrameEventLoader.h>
#include <SteinsGate/Client/ClientTextInfoLoader.h>
#include <SteinsGate/Client/ChannelInfoLoader.h>

//////////////////////////////////////////////////////////////////////////////////////////
DataManager::DataManager()
: DataManagerAbstract()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManager::InitializeLoader()
{
	m_pConfigFileLoaders[ConfigFileType::Effect] = dbg_new EffectInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Map] = dbg_new MapInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::MapPhysics] = dbg_new MapPhysicsInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::MapObject] = dbg_new MapObjectInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Monster] = dbg_new MonsterInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Monster_Animation_Frame_Event] = dbg_new FrameEventLoader(
		this, ActorType::Monster);
	m_pConfigFileLoaders[ConfigFileType::Monster_Projectile] = dbg_new ProjectileInfoLoader(this, ActorType::Monster);
	m_pConfigFileLoaders[ConfigFileType::Monster_Attack_Data] = dbg_new AttackDataInfoLoader(this, ActorType::Monster);

	m_pConfigFileLoaders[ConfigFileType::Server] = dbg_new ServerInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Tile] = dbg_new TileInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::UI] = dbg_new UIInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Action] = dbg_new ActionInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::AI] = dbg_new AIInfoLoader(this);
	// m_pConfigFileLoaders[ConfigFileType::AttackBox]
	m_pConfigFileLoaders[ConfigFileType::Char_Attack_Data] = dbg_new AttackDataInfoLoader(this, ActorType::Character);
	m_pConfigFileLoaders[ConfigFileType::Char_Projectile] = dbg_new ProjectileInfoLoader(this, ActorType::Character);
	m_pConfigFileLoaders[ConfigFileType::Channel] = dbg_new ChannelInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Char_Animation] = dbg_new CharAnimationInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Char_Animation_Frame_Event] = dbg_new FrameEventLoader(
		this, ActorType::Character);
	m_pConfigFileLoaders[ConfigFileType::Char_Base] = dbg_new CharInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Client] = dbg_new ClientInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::ClientText] = dbg_new ClientTextInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Item] = dbg_new ItemInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::ItemOpt] = dbg_new ItemOptInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Char_Common] = dbg_new CharCommonInfoLoader(this);
	// m_pConfigFileLoaders[ConfigFileType::Enchant]					= dbg_new EnchantInfoLoader(this);
	m_bInitialized = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
MonsterInfo* DataManager::GetMonsterInfo(int _mobCode)
{
	auto eType = ConfigFileType::Monster;

	if (!m_bLoaded[eType])
		Load(eType);

	return (MonsterInfo*)GetData(eType, _mobCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ActionInfo* DataManager::GetActionInfo(int _actionCode)
{
	auto eType = ConfigFileType::Action;

	if (!m_bLoaded[eType])
		Load(eType);

	return (ActionInfo*)GetData(eType, _actionCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ProjectileInfo* DataManager::GetProjectileInfo(ActorType_t _actorType, int _projectileCode)
{
	ConfigFileType_t eType = ConfigFileType::Max;

	switch (_actorType)
	{
	case ActorType::Character: eType = ConfigFileType::Char_Projectile;
		break;
	case ActorType::Monster: eType = ConfigFileType::Monster_Projectile;
		break;
	default: DebugAssert(false) ;
	}

	if (!m_bLoaded[eType])
		Load(eType);

	return (ProjectileInfo*)GetData(eType, _projectileCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
CharInfo* DataManager::GetCharInfo(int _charCode)
{
	auto eType = ConfigFileType::Char_Base;

	if (!m_bLoaded[eType])
		Load(eType);

	return (CharInfo*)GetData(eType, _charCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
AnimationInfo* DataManager::GetCharAnimationInfo(int _charAnimationCode)
{
	auto eType = ConfigFileType::Char_Animation;

	if (!m_bLoaded[eType])
		Load(eType);

	return (AnimationInfo*)GetData(eType, _charAnimationCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVector<AnimationInfo*>& DataManager::GetCharAnimationInfoList(int _charCode)
{
	auto eType = ConfigFileType::Char_Animation;

	if (!m_bLoaded[eType])
		Load(eType);

	auto pLoader = (CharAnimationInfoLoader*)m_pConfigFileLoaders[eType];
	return pLoader->GetAnimationList((CharType_t)_charCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ClientInfo* DataManager::GetClientInfo(int _code)
{
	auto eType = ConfigFileType::Client;

	if (!m_bLoaded[eType])
		Load(eType);

	return (ClientInfo*)GetData(eType, _code);
}

//////////////////////////////////////////////////////////////////////////////////////////
TileInfo* DataManager::GetTileInfo(int _tileCode)
{
	auto eType = ConfigFileType::Tile;

	if (!m_bLoaded[eType])
		Load(eType);

	return (TileInfo*)GetData(eType, _tileCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
MapObjectInfo* DataManager::GetMapObjectInfo(int _mapObjectCode)
{
	auto eType = ConfigFileType::MapObject;

	if (!m_bLoaded[eType])
		Load(eType);

	return (MapObjectInfo*)GetData(eType, _mapObjectCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
AIInfo* DataManager::GetAiInfo(int _aiCode)
{
	auto eType = ConfigFileType::AI;

	if (!m_bLoaded[eType])
		Load(eType);

	return (AIInfo*)GetData(eType, _aiCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
AttackDataInfo* DataManager::GetAttackDataInfo(ActorType_t _actorType, int _attackDataCode)
{
	ConfigFileType_t eType = ConfigFileType::Max;

	switch (_actorType)
	{
	case ActorType::Character: eType = ConfigFileType::Char_Attack_Data;
		break;
	case ActorType::Monster: eType = ConfigFileType::Monster_Attack_Data;
		break;
	default: DebugAssert(false) ;
	}

	if (!m_bLoaded[eType])
		Load(eType);

	return (AttackDataInfo*)GetData(eType, _attackDataCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
EffectInfo* DataManager::GetEffectInfo(int _effectCode)
{
	auto eType = ConfigFileType::Effect;

	if (!m_bLoaded[eType])
		Load(eType);

	return (EffectInfo*)GetData(eType, _effectCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* DataManager::GetUiElementInfo(int _uiElementCode)
{
	auto eType = ConfigFileType::UI;

	if (!m_bLoaded[eType])
		Load(eType);

	return (UIElementInfo*)GetData(eType, _uiElementCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
FrameEvent* DataManager::GetFrameEvent(ActorType_t _actorType, int _frameEventCode)
{
	ConfigFileType_t eType = ConfigFileType::Max;

	switch (_actorType)
	{
	case ActorType::Character: eType = ConfigFileType::Char_Animation_Frame_Event;
		break;
	case ActorType::Monster: eType = ConfigFileType::Monster_Animation_Frame_Event;
		break;
	default: DebugAssert(false);
	}

	if (!m_bLoaded[eType])
		Load(eType);

	return (FrameEvent*)GetData(eType, _frameEventCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ChannelInfo* DataManager::GetChannelInfo(int _channelCode)
{
	auto eType = ConfigFileType::Channel;

	if (!m_bLoaded[eType])
		Load(eType);

	return (ChannelInfo*)GetData(eType, _channelCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
char* DataManager::GetTextRaw(const char* _textId)
{
	auto eType = ConfigFileType::ClientText;

	if (!m_bLoaded[eType])
		Load(eType);

	const auto pLoader = dynamic_cast<ClientTextInfoLoader*>(m_pConfigFileLoaders[eType]);

	if (pLoader == nullptr)
	{
		_LogWarn_("텍스트 로더가 없습니다.");
		return ClientTextInfoLoader::DummyText.Source();
	}

	char* pText = nullptr;
	if (!pLoader->TryGetTextRaw(_textId, &pText))
	{
		_LogWarn_("%s 텍스트를 찾지 못했습니다.", _textId);
	}

	return pText;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString& DataManager::GetText(const char* _textId)
{
	auto eType = ConfigFileType::ClientText;

	if (!m_bLoaded[eType])
		Load(eType);

	const auto pLoader = dynamic_cast<ClientTextInfoLoader*>(m_pConfigFileLoaders[eType]);

	if (pLoader == nullptr)
	{
		_LogWarn_("텍스트 로더가 없습니다.");
		return ClientTextInfoLoader::DummyText;
	}

	SGString* pText = nullptr;
	if (!pLoader->TryGetText(_textId, &pText))
	{
		_LogWarn_("%s 텍스트를 찾지 못했습니다.", _textId);
	}

	return *pText;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString& DataManager::GetText(const SGString& _textId)
{
	return GetText(_textId.Source());
}
