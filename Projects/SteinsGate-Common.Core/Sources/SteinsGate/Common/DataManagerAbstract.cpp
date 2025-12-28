/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 6:40:01 PM
 * =====================
 *
 */


#include "Core.h"
#include "DataManagerAbstract.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/CommonInfoLoader.h>
#include <SteinsGate/Common/ItemOptInfoLoader.h>
#include <SteinsGate/Common/ChannelBaseInfoLoader.h>
#include <SteinsGate/Common/EnchantInfoLoader.h>
#include <SteinsGate/Common/ServerInfoLoader.h>
#include <SteinsGate/Common/MapInfoLoader.h>

//////////////////////////////////////////////////////////////////////////////////////////
DataManagerAbstract::DataManagerAbstract()
: m_pConfigFileLoaders{}
, m_bLoaded{}
, m_bInitialized(false)
{
	LoadCommon();
}

//////////////////////////////////////////////////////////////////////////////////////////
DataManagerAbstract::~DataManagerAbstract()
{
	FinalizeLoader();
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManagerAbstract::LoadCommon()
{
	DebugAssertMsg(m_bLoaded[ConfigFileType::Common] == false, "이미 초기화가 진행되어있습니다.");

	CommonInfoLoader* pCommonInfoLoader = dbg_new CommonInfoLoader(this);

	if (!pCommonInfoLoader->Load())
	{
		DebugAssertMsg(false, "커몬 인포 로딩에 실패했습니다.");
		return;
	}

	m_pConfigFileLoaders[ConfigFileType::Common] = pCommonInfoLoader;
	m_bLoaded[ConfigFileType::Common] = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManagerAbstract::LoadAll()
{
	DebugAssertMsg(m_bInitialized, "아직 데이터 매니저 초기화가 이뤄지지 않았습니다.");

	int initCount = 0;
	int loadedCount = 0;

	for (int configFileTypeIndex = 0; configFileTypeIndex < ConfigFileType::Max; ++configFileTypeIndex)
	{
		if (m_pConfigFileLoaders[configFileTypeIndex] == nullptr)
			continue;

		++initCount;

		if (m_bLoaded[configFileTypeIndex])
			continue;

		Load((ConfigFileType_t)configFileTypeIndex);

		if (m_bLoaded[configFileTypeIndex])
		{
			++loadedCount;
		}
	}

	_LogInfo_("기획파일 %d개중 %d개를 로딩하였습니다.", initCount, loadedCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
ConfigDataAbstract* DataManagerAbstract::GetData(ConfigFileType_t _configFileType, int _code)
{
	DebugAssertMsg(_configFileType >= ConfigFileType::Begin && _configFileType <= ConfigFileType::End
	               , "올바르지 않은 ConfigFileType 입니다.");
	return m_pConfigFileLoaders[_configFileType]->GetData(_code);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManagerAbstract::Load(ConfigFileType_t _configFileType)
{
	ConfigFileLoaderAbstract* pLoader = m_pConfigFileLoaders[_configFileType];
	DebugAssertMsg(pLoader != nullptr, "%s 파일 로더가 아직 생성되어있지 않습니다.", ConfigFileType::FileName[_configFileType]);
	m_bLoaded[_configFileType] = pLoader->Load();
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManagerAbstract::Unload(ConfigFileType_t _configFileType)
{
	DebugAssertMsg(false, "아직 구현 안됨");
	// TODO: 필요시 구현
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManagerAbstract::FinalizeLoader()
{
	for (int configFileTypeIndex = 0; configFileTypeIndex < ConfigFileType::Max; ++configFileTypeIndex)
	{
		JCORE_DELETE_SAFE(m_pConfigFileLoaders[configFileTypeIndex]);
		m_bLoaded[configFileTypeIndex] = false;
	}

	m_bInitialized = false;

	_LogDebug_("설정파일에서 읽은 모든 데이터를 정리하였습니다.");
}


// ========================================================================================
// 공통 데이터 가져오기
// ========================================================================================

//////////////////////////////////////////////////////////////////////////////////////////
MonsterBaseInfo* DataManagerAbstract::GetMobBaseInfo(int _monsterCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Monster;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<MonsterBaseInfo*>(GetData(CONFIG_TYPE, _monsterCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemAvatarInfo* DataManagerAbstract::GetAvatarInfo(int _avatarCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Item;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<ItemAvatarInfo*>(GetData(CONFIG_TYPE, _avatarCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemWeaponInfo* DataManagerAbstract::GetWeaponInfo(int _weaponCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Item;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<ItemWeaponInfo*>(GetData(CONFIG_TYPE, _weaponCode));
	DebugAssertMsg(pRet, "무기 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemArmorInfo* DataManagerAbstract::GetArmorInfo(int _armorCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Item;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<ItemArmorInfo*>(GetData(CONFIG_TYPE, _armorCode));
	DebugAssertMsg(pRet, "방어구 | 장신구 | 칭호 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemVisualInfo* DataManagerAbstract::GetVisualInfo(int _visualCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Item;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<ItemVisualInfo*>(GetData(CONFIG_TYPE, _visualCode));
	DebugAssertMsg(pRet, "아바타 | 무기가 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
CommonInfo* DataManagerAbstract::GetCommonInfo(int _commonConfigCode)
{
	DebugAssertMsg(m_bLoaded[ConfigFileType::Common], "먼저 loadCommon()을 호출해주세요");
	return (CommonInfo*)m_pConfigFileLoaders[ConfigFileType::Common]->GetData(_commonConfigCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemOptInfo* DataManagerAbstract::GetItemOptInfo(int _itemOptCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::ItemOpt;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<ItemOptInfo*>(GetData(CONFIG_TYPE, _itemOptCode));
	DebugAssertMsg(pRet, "아이템 옵트 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemOptInfo* DataManagerAbstract::GetItemOptInfo(const SGString& _itemOptEngName)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::ItemOpt;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	return ((ItemOptInfoLoader*)m_pConfigFileLoaders[CONFIG_TYPE])->GetData(_itemOptEngName);
}

//////////////////////////////////////////////////////////////////////////////////////////
ChannelBaseInfo* DataManagerAbstract::GetChannelBaseInfo(int _channelCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Channel;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<ChannelBaseInfo*>(GetData(CONFIG_TYPE, _channelCode));
	DebugAssertMsg(pRet, "채널 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
EnchantInfo* DataManagerAbstract::GetEnchantInfo(int _enchantCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Enchant;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<EnchantInfo*>(GetData(CONFIG_TYPE, _enchantCode));
	DebugAssertMsg(pRet, "인챈트 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ServerProcessInfoPackage* DataManagerAbstract::GetServerProcessInfoPackage(int _serverCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Server;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<ServerProcessInfoPackage*>(GetData(CONFIG_TYPE, _serverCode));
	DebugAssertMsg(pRet, "서버 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
DatabaseInfo* DataManagerAbstract::GetDatabaseInfo(int _databaseCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Database;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<DatabaseInfo*>(GetData(CONFIG_TYPE, _databaseCode));
	DebugAssertMsg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
CharCommonInfo* DataManagerAbstract::GetCharCommonInfo(int _charCommonCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Char_Common;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<CharCommonInfo*>(GetData(CONFIG_TYPE, _charCommonCode));
	DebugAssertMsg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapInfo* DataManagerAbstract::GetMapInfo(int _mapCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Map;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<MapInfo*>(GetData(CONFIG_TYPE, _mapCode));
	DebugAssertMsg(pRet, "맵 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapAreaInfo* DataManagerAbstract::GetMapAreaInfo(int _mapCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::Map;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

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
MapPhysicsInfo* DataManagerAbstract::GetMapPhysicsInfo(int _physicsCode)
{
	constexpr auto CONFIG_TYPE = ConfigFileType::MapPhysics;

	if (!m_bLoaded[CONFIG_TYPE])
		Load(CONFIG_TYPE);

	const auto pRet = dynamic_cast<MapPhysicsInfo*>(GetData(CONFIG_TYPE, _physicsCode));
	DebugAssertMsg(pRet, "맵 피직스 타입이 아닙니다.");
	return pRet;
}
