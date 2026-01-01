/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:01:29 PM
 * =====================
 *
 */

#include "Core.h"
#include "DataManager.h"

#include <sg/ItemOptInfoLoader.h>
#include <sg/ChannelBaseInfoLoader.h>
#include <sg/EnchantInfoLoader.h>
#include <sg/CharCommonInfoLoader.h>
#include <sg/ServerInfoLoader.h>
#include <sg/ItemInfoLoader.h>
#include <sg/MapInfoLoader.h>
#include <sg/MapPhysicsInfoLoader.h>

#include <sgcl/ActionInfoLoader.h>
#include <sgcl/MonsterInfoLoader.h>
#include <sgcl/CharInfoLoader.h>
#include <sgcl/ProjectileInfoLoader.h>
#include <sgcl/TileInfoLoader.h>
#include <sgcl/MapObjectInfoLoader.h>
#include <sgcl/AIInfoLoader.h>
#include <sgcl/AttackDataInfoLoader.h>
#include <sgcl/CharAnimationInfoLoader.h>
#include <sgcl/EffectInfoLoader.h>
#include <sgcl/UIInfoLoader.h>
#include <sgcl/FrameEventLoader.h>
#include <sgcl/ClientTextInfoLoader.h>
#include <sgcl/ChannelInfoLoader.h>

//////////////////////////////////////////////////////////////////////////////////////////
DataManager::DataManager()
: DescLoaderMgr()
{
	m_pConfigFileLoaders[ConfigFileType::Effect] = dbg_new EffectInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Map] = dbg_new MapInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::MapPhysics] = dbg_new MapPhysicsInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::MapObject] = dbg_new MapObjectInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Monster] = dbg_new MonsterInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Monster_Animation_Frame_Event] = dbg_new FrameEventLoader(this, ActorType::Monster);
	m_pConfigFileLoaders[ConfigFileType::Monster_Projectile] = dbg_new ProjectileInfoLoader(this, ActorType::Monster);
	m_pConfigFileLoaders[ConfigFileType::Monster_Attack_Data] = dbg_new AttackDataInfoLoader(this, ActorType::Monster);
	m_pConfigFileLoaders[ConfigFileType::Server] = dbg_new ServerInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Tile] = dbg_new TileInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::UI] = dbg_new UIInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Action] = dbg_new ActionInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::AI] = dbg_new AIInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Char_Attack_Data] = dbg_new AttackDataInfoLoader(this, ActorType::Character);
	m_pConfigFileLoaders[ConfigFileType::Char_Projectile] = dbg_new ProjectileInfoLoader(this, ActorType::Character);
	m_pConfigFileLoaders[ConfigFileType::Channel] = dbg_new ChannelInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Char_Animation] = dbg_new CharAnimationInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Char_Animation_Frame_Event] = dbg_new FrameEventLoader(this, ActorType::Character);
	m_pConfigFileLoaders[ConfigFileType::Char_Base] = dbg_new CharInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::ClientText] = dbg_new ClientTextInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Item] = dbg_new ItemInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::ItemOpt] = dbg_new ItemOptInfoLoader(this);
	m_pConfigFileLoaders[ConfigFileType::Char_Common] = dbg_new CharCommonInfoLoader(this);
}

//////////////////////////////////////////////////////////////////////////////////////////
MonsterInfo* DataManager::GetMonsterInfo(int _mobCode)
{
	constexpr auto eType = ConfigFileType::Monster;
	return (MonsterInfo*)GetData(eType, _mobCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ActionInfo* DataManager::GetActionInfo(int _actionCode)
{
	constexpr auto eType = ConfigFileType::Action;
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
	default: jc_assert(false) ;
	}

	return (ProjectileInfo*)GetData(eType, _projectileCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
CharInfo* DataManager::GetCharInfo(int _charCode)
{
	constexpr auto eType = ConfigFileType::Char_Base;
	return (CharInfo*)GetData(eType, _charCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
AnimationInfo* DataManager::GetCharAnimationInfo(int _charAnimationCode)
{
	constexpr auto eType = ConfigFileType::Char_Animation;
	return (AnimationInfo*)GetData(eType, _charAnimationCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::Vector<AnimationInfo*>& DataManager::GetCharAnimationInfoList(int _charCode)
{
	constexpr auto eType = ConfigFileType::Char_Animation;
	auto pLoader = (CharAnimationInfoLoader*)m_pConfigFileLoaders[eType];
	return pLoader->GetAnimationList((CharType_t)_charCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
TileInfo* DataManager::GetTileInfo(int _tileCode)
{
	constexpr auto eType = ConfigFileType::Tile;
	return (TileInfo*)GetData(eType, _tileCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
MapObjectInfo* DataManager::GetMapObjectInfo(int _mapObjectCode)
{
	constexpr auto eType = ConfigFileType::MapObject;
	return (MapObjectInfo*)GetData(eType, _mapObjectCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
AIInfo* DataManager::GetAiInfo(int _aiCode)
{
	constexpr auto eType = ConfigFileType::AI;
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
	default: jc_assert(false) ;
	}
	return (AttackDataInfo*)GetData(eType, _attackDataCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
EffectInfo* DataManager::GetEffectInfo(int _effectCode)
{
	constexpr auto eType = ConfigFileType::Effect;
	return (EffectInfo*)GetData(eType, _effectCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* DataManager::GetUIElementInfo(int _uiElementCode)
{
	constexpr auto eType = ConfigFileType::UI;
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
	default: jc_assert(false);
	}

	return (FrameEvent*)GetData(eType, _frameEventCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ChannelInfo* DataManager::GetChannelInfo(int _channelCode)
{
	constexpr auto eType = ConfigFileType::Channel;
	return (ChannelInfo*)GetData(eType, _channelCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
char* DataManager::GetTextRaw(const char* _textId)
{
	constexpr auto eType = ConfigFileType::ClientText;
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
jc::String& DataManager::GetText(const char* _textId)
{
	constexpr auto eType = ConfigFileType::ClientText;
	const auto pLoader = dynamic_cast<ClientTextInfoLoader*>(m_pConfigFileLoaders[eType]);

	if (pLoader == nullptr)
	{
		_LogWarn_("텍스트 로더가 없습니다.");
		return ClientTextInfoLoader::DummyText;
	}

	jc::String* pText = nullptr;
	if (!pLoader->TryGetText(_textId, &pText))
	{
		_LogWarn_("%s 텍스트를 찾지 못했습니다.", _textId);
	}

	return *pText;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String& DataManager::GetText(const jc::String& _textId)
{
	return GetText(_textId.Source());
}
