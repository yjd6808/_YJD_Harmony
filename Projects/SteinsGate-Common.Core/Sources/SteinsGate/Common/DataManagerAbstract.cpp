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
	loadCommon();
}

//////////////////////////////////////////////////////////////////////////////////////////
DataManagerAbstract::~DataManagerAbstract()
{
	finalizeLoader();
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManagerAbstract::loadCommon()
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
void DataManagerAbstract::loadAll()
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

		load((ConfigFileType_t)configFileTypeIndex);

		if (m_bLoaded[configFileTypeIndex])
		{
			++loadedCount;
		}
	}

	_LogInfo_("기획파일 %d개중 %d개를 로딩하였습니다.", initCount, loadedCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
ConfigDataAbstract* DataManagerAbstract::getData(ConfigFileType_t _configFileType, int _code)
{
	DebugAssertMsg(_configFileType >= ConfigFileType::Begin && _configFileType <= ConfigFileType::End
	               , "올바르지 않은 ConfigFileType 입니다.");
	return m_pConfigFileLoaders[_configFileType]->GetData(_code);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManagerAbstract::load(ConfigFileType_t _configFileType)
{
	ConfigFileLoaderAbstract* pLoader = m_pConfigFileLoaders[_configFileType];
	DebugAssertMsg(pLoader != nullptr, "%s 파일 로더가 아직 생성되어있지 않습니다.", ConfigFileType::FileName[_configFileType]);
	m_bLoaded[_configFileType] = pLoader->Load();
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManagerAbstract::unload(ConfigFileType_t _configFileType)
{
	DebugAssertMsg(false, "아직 구현 안됨");
	// TODO: 필요시 구현
}

//////////////////////////////////////////////////////////////////////////////////////////
void DataManagerAbstract::finalizeLoader()
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
MonsterBaseInfo* DataManagerAbstract::getMobBaseInfo(int _monsterCode)
{
	const auto configType = ConfigFileType::Monster;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<MonsterBaseInfo*>(getData(configType, _monsterCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemAvatarInfo* DataManagerAbstract::getAvatarInfo(int _avatarCode)
{
	const auto configType = ConfigFileType::Item;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<ItemAvatarInfo*>(getData(configType, _avatarCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemWeaponInfo* DataManagerAbstract::getWeaponInfo(int _weaponCode)
{
	const auto configType = ConfigFileType::Item;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<ItemWeaponInfo*>(getData(configType, _weaponCode));
	DebugAssertMsg(pRet, "무기 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemArmorInfo* DataManagerAbstract::getArmorInfo(int _armorCode)
{
	const auto configType = ConfigFileType::Item;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<ItemArmorInfo*>(getData(configType, _armorCode));
	DebugAssertMsg(pRet, "방어구 | 장신구 | 칭호 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemVisualInfo* DataManagerAbstract::getVisualInfo(int _visualCode)
{
	const auto configType = ConfigFileType::Item;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<ItemVisualInfo*>(getData(configType, _visualCode));
	DebugAssertMsg(pRet, "아바타 | 무기가 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
CommonInfo* DataManagerAbstract::getCommonInfo(int _commonConfigCode)
{
	DebugAssertMsg(m_bLoaded[ConfigFileType::Common], "먼저 loadCommon()을 호출해주세요");
	return (CommonInfo*)m_pConfigFileLoaders[ConfigFileType::Common]->GetData(_commonConfigCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemOptInfo* DataManagerAbstract::getItemOptInfo(int _itemOptCode)
{
	const auto configType = ConfigFileType::ItemOpt;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<ItemOptInfo*>(getData(configType, _itemOptCode));
	DebugAssertMsg(pRet, "아이템 옵트 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemOptInfo* DataManagerAbstract::getItemOptInfo(const SGString& _itemOptEngName)
{
	const auto configType = ConfigFileType::ItemOpt;

	if (!m_bLoaded[configType])
		load(configType);

	return ((ItemOptInfoLoader*)m_pConfigFileLoaders[configType])->GetData(_itemOptEngName);
}

//////////////////////////////////////////////////////////////////////////////////////////
ChannelBaseInfo* DataManagerAbstract::getChannelBaseInfo(int _channelCode)
{
	const auto configType = ConfigFileType::Channel;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<ChannelBaseInfo*>(getData(configType, _channelCode));
	DebugAssertMsg(pRet, "채널 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
EnchantInfo* DataManagerAbstract::getEnchantInfo(int _enchantCode)
{
	const auto configType = ConfigFileType::Enchant;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<EnchantInfo*>(getData(configType, _enchantCode));
	DebugAssertMsg(pRet, "인챈트 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
ServerProcessInfoPackage* DataManagerAbstract::getServerProcessInfoPackage(int _serverCode)
{
	const auto configType = ConfigFileType::Server;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<ServerProcessInfoPackage*>(getData(configType, _serverCode));
	DebugAssertMsg(pRet, "서버 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
DatabaseInfo* DataManagerAbstract::getDatabaseInfo(int _databaseCode)
{
	const auto configType = ConfigFileType::Database;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<DatabaseInfo*>(getData(configType, _databaseCode));
	DebugAssertMsg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
CharCommonInfo* DataManagerAbstract::getCharCommonInfo(int _charCommonCode)
{
	const auto configType = ConfigFileType::Char_Common;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<CharCommonInfo*>(getData(configType, _charCommonCode));
	DebugAssertMsg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapInfo* DataManagerAbstract::getMapInfo(int _mapCode)
{
	const auto configType = ConfigFileType::Map;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<MapInfo*>(getData(configType, _mapCode));
	DebugAssertMsg(pRet, "맵 인포 타입이 아닙니다.");
	return pRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapAreaInfo* DataManagerAbstract::getMapAreaInfo(int _mapCode)
{
	const auto configType = ConfigFileType::Map;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pLoader = dynamic_cast<MapInfoLoader*>(m_pConfigFileLoaders[configType]);

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
MapPhysicsInfo* DataManagerAbstract::getMapPhysicsInfo(int _physicsCode)
{
	const auto configType = ConfigFileType::MapPhysics;

	if (!m_bLoaded[configType])
		load(configType);

	const auto pRet = dynamic_cast<MapPhysicsInfo*>(getData(configType, _physicsCode));
	DebugAssertMsg(pRet, "맵 피직스 타입이 아닙니다.");
	return pRet;
}
