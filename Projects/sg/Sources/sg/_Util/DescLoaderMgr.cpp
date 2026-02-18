/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 6:40:01 PM
 * =====================
 *
 */


#include "Core.h"
#include "DescLoaderMgr.h"

#include <sg/_Struct/SteinsGate_ChannelBase.h>
#include <sg/_Struct/SteinsGate_Enchant.h>
#include <sg/_Struct/SteinsGate_Server.h>
#include <sg/_Struct/SteinsGate_MonsterBase.h>
#include <sg/_Struct/SteinsGate_Item.h>
#include <sg/_Struct/SteinsGate_MapPhysics.h>
#include <sg/_Struct/SteinsGate_Database.h>
#include <sg/_Struct/SteinsGate_CharCommon.h>

#include <sg/_Struct/SteinsGate_Action.h>
#include <sg/_Struct/SteinsGate_Char.h>
#include <sg/_Struct/SteinsGate_Tile.h>
#include <sg/_Struct/SteinsGate_AI.h>
#include <sg/_Struct/SteinsGate_AttackData.h>
#include <sg/_Struct/SteinsGate_Effect.h>
#include <sg/_Struct/SteinsGate_UI.h>
#include <sg/_Struct/SteinsGate_FrameEvent.h>
#include <sg/_Struct/SteinsGate_Channel.h>

#include <sg/_Util/_DescMgr/DescMgr_Map.h>
#include <sg/_Util/_DescMgr/DescMgr_ItemOpt.h>
#include <sg/_Util/_DescMgr/DescMgr_ClientText.h>
#include <sg/_Util/_DescMgr/DescMgr_CharAnimation.h>

//////////////////////////////////////////////////////////////////////////////////////////
DescLoaderMgr::DescLoaderMgr()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
DescLoaderMgr::~DescLoaderMgr()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderMgr::LoadAll()
{
	int initCount = 0;
	int loadedCount = 0;

	for (int configFileTypeIndex = 0; configFileTypeIndex < ConfigFileType::Max; ++configFileTypeIndex)
	{
		auto loader = m_pConfigFileLoaders[configFileTypeIndex];
		if (loader == nullptr)
			continue;

		++initCount;

		if (loader->IsLoaded())
			continue;

		if (loader->Load())
		{
			++loadedCount;
		}
	}

	_LogInfo_("기획파일 %d개중 %d개를 로딩하였습니다.", initCount, loadedCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
SDescBase* DescLoaderMgr::GetData(ConfigFileType_t _configFileType, int _code)
{
	jc_assert(m_pConfigFileLoaders[_configFileType] != nullptr);
	jc_assert_msg(_configFileType >= ConfigFileType::Begin && _configFileType <= ConfigFileType::End, "올바르지 않은 ConfigFileType 입니다.");
	return m_pConfigFileLoaders[_configFileType]->GetData(_code);
}

//////////////////////////////////////////////////////////////////////////////////////////
SDescBase* DescLoaderMgr::GetDataAny(ConfigFileType_t _configFileType)
{
	jc_assert(m_pConfigFileLoaders[_configFileType] != nullptr);
	jc_assert_msg(_configFileType >= ConfigFileType::Begin && _configFileType <= ConfigFileType::End, "올바르지 않은 ConfigFileType 입니다.");
	return m_pConfigFileLoaders[_configFileType]->GetDataAny();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool DescLoaderMgr::Load(ConfigFileType_t _configFileType)
{
	DescLoaderAbstract* pLoader = m_pConfigFileLoaders[_configFileType];
	jc_assert_msg(pLoader != nullptr, "%s 파일 로더가 아직 생성되어있지 않습니다.", ConfigFileType::FileName[_configFileType]);
	if (!pLoader->IsLoaded())
	{
		return pLoader->Load();
	}	
	return false;
	
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderMgr::Unload(ConfigFileType_t _configFileType)
{
	jc_assert_msg(false, "아직 구현 안됨");
	// TODO: 필요시 구현
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderMgr::Clear()
{
	for (int configFileTypeIndex = 0; configFileTypeIndex < ConfigFileType::Max; ++configFileTypeIndex)
	{
		JC_DELETE_SAFE(m_pConfigFileLoaders[configFileTypeIndex]);
	}

	_LogDebug_("설정파일에서 읽은 모든 데이터를 정리하였습니다.");
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderMgr::AddLoader(DescLoaderAbstract* _pLoader)
{
	jc_assert_msg(_pLoader != nullptr, "널 포인터를 추가할 수 없습니다.");
	const ConfigFileType_t configFileType = _pLoader->GetConfigFileType();
	if (m_pConfigFileLoaders[configFileType] != nullptr)
	{
		_LogWarn_("이미 %s 파일 로더가 존재합니다.", ConfigFileType::FileName[configFileType]);
		m_pConfigFileLoaders[configFileType]->Unload();
		JC_DELETE_SAFE(m_pConfigFileLoaders[configFileType]);
	}
	m_pConfigFileLoaders[configFileType] = _pLoader;
}


// ========================================================================================
// 공통 데이터 가져오기
// ========================================================================================

//////////////////////////////////////////////////////////////////////////////////////////
MonsterBaseInfo* DescLoaderMgr::GetMobBaseInfo(int _monsterCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Monster;
	const auto pRet = dynamic_cast<MonsterBaseInfo*>(GetData(CONFIG_TYPE, _monsterCode));
	jc_assert_msg(pRet, "아바타 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemAvatarInfo* DescLoaderMgr::GetAvatarInfo(int _avatarCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Item;
	const auto pRet = dynamic_cast<ItemAvatarInfo*>(GetData(CONFIG_TYPE, _avatarCode));
	jc_assert_msg(pRet, "아바타 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemWeaponInfo* DescLoaderMgr::GetWeaponInfo(int _weaponCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Item;
	const auto pRet = dynamic_cast<ItemWeaponInfo*>(GetData(CONFIG_TYPE, _weaponCode));
	jc_assert_msg(pRet, "무기 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemArmorInfo* DescLoaderMgr::GetArmorInfo(int _armorCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Item;
	const auto pRet = dynamic_cast<ItemArmorInfo*>(GetData(CONFIG_TYPE, _armorCode));
	jc_assert_msg(pRet, "방어구 | 장신구 | 칭호 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemVisualInfo* DescLoaderMgr::GetVisualInfo(int _visualCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Item;
	const auto pRet = dynamic_cast<ItemVisualInfo*>(GetData(CONFIG_TYPE, _visualCode));
	jc_assert_msg(pRet, "아바타 | 무기가 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemOptInfo* DescLoaderMgr::GetItemOptInfo(int _itemOptCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::ItemOpt;
	const auto pRet = dynamic_cast<ItemOptInfo*>(GetData(CONFIG_TYPE, _itemOptCode));
	jc_assert_msg(pRet, "아이템 옵트 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemOptInfo* DescLoaderMgr::GetItemOptInfo(const jc::String& _itemOptEngName)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::ItemOpt;
	return ((ItemOptInfoLoader*)m_pConfigFileLoaders[CONFIG_TYPE])->GetData(_itemOptEngName);
}

//////////////////////////////////////////////////////////////////////////////////////////
ChannelBaseInfo* DescLoaderMgr::GetChannelBaseInfo(int _channelCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Channel;
	const auto pRet = dynamic_cast<ChannelBaseInfo*>(GetData(CONFIG_TYPE, _channelCode));
	jc_assert_msg(pRet, "채널 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
EnchantInfo* DescLoaderMgr::GetEnchantInfo(int _enchantCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Enchant;
	const auto pRet = dynamic_cast<EnchantInfo*>(GetData(CONFIG_TYPE, _enchantCode));
	jc_assert_msg(pRet, "인챈트 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ServerProcessInfo* DescLoaderMgr::GetServerProcessInfo()
{
	constexpr auto CONFIG_TYPE = ConfigFileType::ServerInfo;
	const auto pRet = dynamic_cast<ServerProcessInfo*>(GetDataAny(CONFIG_TYPE));
	jc_assert_msg(pRet, "서버 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
DatabaseInfo* DescLoaderMgr::GetDatabaseInfo(int _databaseCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Database;
	const auto pRet = dynamic_cast<DatabaseInfo*>(GetData(CONFIG_TYPE, _databaseCode));
	jc_assert_msg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
CharCommonInfo* DescLoaderMgr::GetCharCommonInfo(int _charCommonCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Char_Common;
	const auto pRet = dynamic_cast<CharCommonInfo*>(GetData(CONFIG_TYPE, _charCommonCode));
	jc_assert_msg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapInfo* DescLoaderMgr::GetMapInfo(int _mapCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Map;
	const auto pRet = dynamic_cast<MapInfo*>(GetData(CONFIG_TYPE, _mapCode));
	jc_assert_msg(pRet, "맵 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapAreaInfo* DescLoaderMgr::GetMapAreaInfo(int _mapCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Map;
	const auto pLoader = dynamic_cast<MapInfoLoader*>(m_pConfigFileLoaders[CONFIG_TYPE]);

	if (pLoader == nullptr)
	{
		_LogWarn_("맵 인포 로더가 없습니다.");
		return nullptr;
	}

	MapAreaInfo* pRet = pLoader->GetMapAreaInfo(_mapCode);
	if (pRet == nullptr)
	{
		_LogWarn_("%d 맵의 Area 데이터를 찾지 못했습니다.", _mapCode);
		return nullptr;
	}

	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapPhysicsInfo* DescLoaderMgr::GetMapPhysicsInfo(int _physicsCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::MapPhysics;
	const auto pRet = dynamic_cast<MapPhysicsInfo*>(GetData(CONFIG_TYPE, _physicsCode));
	jc_assert_msg(pRet, "맵 피직스 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
MonsterInfo* DescLoaderMgr::GetMonsterInfo(int _mobCode)
{
	constexpr auto eType = ConfigFileType::Monster;
	return (MonsterInfo*)GetData(eType, _mobCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ActionInfo* DescLoaderMgr::GetActionInfo(int _actionCode)
{
	constexpr auto eType = ConfigFileType::Action;
	return (ActionInfo*)GetData(eType, _actionCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ProjectileInfo* DescLoaderMgr::GetProjectileInfo(ActorType_t _actorType, int _projectileCode)
{
	ConfigFileType_t eType = ConfigFileType::Max;

	switch (_actorType)
	{
	case ActorType::Character: eType = ConfigFileType::Char_Projectile;
		break;
	case ActorType::Monster: eType = ConfigFileType::Monster_Projectile;
		break;
	default: jc_assert(false);
	}

	return (ProjectileInfo*)GetData(eType, _projectileCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
CharInfo* DescLoaderMgr::GetCharInfo(int _charCode)
{
	constexpr auto eType = ConfigFileType::Char_Base;
	return (CharInfo*)GetData(eType, _charCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
AnimationInfo* DescLoaderMgr::GetCharAnimationInfo(int _charAnimationCode)
{
	constexpr auto eType = ConfigFileType::Char_Animation;
	return (AnimationInfo*)GetData(eType, _charAnimationCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::Vector<AnimationInfo*>& DescLoaderMgr::GetCharAnimationInfoList(int _charCode)
{
	constexpr auto eType = ConfigFileType::Char_Animation;
	auto pLoader = (CharAnimationInfoLoader*)m_pConfigFileLoaders[eType];
	return pLoader->GetAnimationList((CharType_t)_charCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
TileInfo* DescLoaderMgr::GetTileInfo(int _tileCode)
{
	constexpr auto eType = ConfigFileType::Tile;
	return (TileInfo*)GetData(eType, _tileCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
MapObjectInfo* DescLoaderMgr::GetMapObjectInfo(int _mapObjectCode)
{
	constexpr auto eType = ConfigFileType::MapObject;
	return (MapObjectInfo*)GetData(eType, _mapObjectCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
AIInfo* DescLoaderMgr::GetAiInfo(int _aiCode)
{
	constexpr auto eType = ConfigFileType::AI;
	return (AIInfo*)GetData(eType, _aiCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
AttackDataInfo* DescLoaderMgr::GetAttackDataInfo(ActorType_t _actorType, int _attackDataCode)
{
	ConfigFileType_t eType = ConfigFileType::Max;

	switch (_actorType)
	{
	case ActorType::Character: eType = ConfigFileType::Char_Attack_Data;
		break;
	case ActorType::Monster: eType = ConfigFileType::Monster_Attack_Data;
		break;
	default: jc_assert(false);
	}
	return (AttackDataInfo*)GetData(eType, _attackDataCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
EffectInfo* DescLoaderMgr::GetEffectInfo(int _effectCode)
{
	constexpr auto eType = ConfigFileType::Effect;
	return (EffectInfo*)GetData(eType, _effectCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* DescLoaderMgr::GetUIElementInfo(int _uiElementCode)
{
	constexpr auto eType = ConfigFileType::UI;
	return (UIElementInfo*)GetData(eType, _uiElementCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
FrameEvent* DescLoaderMgr::GetFrameEvent(ActorType_t _actorType, int _frameEventCode)
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
ChannelInfo* DescLoaderMgr::GetChannelInfo(int _channelCode)
{
	constexpr auto eType = ConfigFileType::Channel;
	return (ChannelInfo*)GetData(eType, _channelCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
char* DescLoaderMgr::GetTextRaw(const char* _textId)
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
jc::String& DescLoaderMgr::GetText(const char* _textId)
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
jc::String& DescLoaderMgr::GetText(const jc::String& _textId)
{
	return GetText(_textId.Source());
}