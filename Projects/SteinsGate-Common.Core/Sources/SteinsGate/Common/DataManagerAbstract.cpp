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

DataManagerAbstract::DataManagerAbstract()
	: m_pConfigFileLoaders{}
	, m_bLoaded{}
	, m_bInitialized(false)
{
	loadCommon();	
}

DataManagerAbstract::~DataManagerAbstract() {
	finalizeLoader();
}

void DataManagerAbstract::loadCommon() {

	DebugAssertMsg(m_bLoaded[ConfigFileType::Common] == false, "이미 초기화가 진행되어있습니다.");

	CommonInfoLoader* pCommonInfoLoader = dbg_new CommonInfoLoader(this);

	if (!pCommonInfoLoader->load()) {
		DebugAssertMsg(false, "커몬 인포 로딩에 실패했습니다.");
		return;
	}

	m_pConfigFileLoaders[ConfigFileType::Common] = pCommonInfoLoader;
	m_bLoaded[ConfigFileType::Common] = true;
}

void DataManagerAbstract::loadAll() {
	DebugAssertMsg(m_bInitialized, "아직 데이터 매니저 초기화가 이뤄지지 않았습니다.");

	int iInitCount = 0;
	int iLoadedCount = 0;

	for (int i = 0; i < ConfigFileType::Max; ++i) {
		if (m_pConfigFileLoaders[i] == nullptr)
			continue;

		++iInitCount;


		if (m_bLoaded[i])
			continue;

		load((ConfigFileType_t)i);

		if (m_bLoaded[i]) {
			++iLoadedCount;
		}
	}

	_LogInfo_("기획파일 %d개중 %d개를 로딩하였습니다.", iInitCount, iLoadedCount);
}

ConfigDataAbstract* DataManagerAbstract::getData(ConfigFileType_t configFileType, int code) {
	DebugAssertMsg(configFileType >= ConfigFileType::Begin && configFileType <= ConfigFileType::End
		, "올바르지 않은 ConfigFileType 입니다.");
	return m_pConfigFileLoaders[configFileType]->getData(code);
}

void DataManagerAbstract::load(ConfigFileType_t configFileType) {
	ConfigFileLoaderAbstract* pLoader = m_pConfigFileLoaders[configFileType];
	DebugAssertMsg(pLoader != nullptr, "%s 파일 로더가 아직 생성되어있지 않습니다.", ConfigFileType::FileName[configFileType]);
	m_bLoaded[configFileType] = pLoader->load();
}

void DataManagerAbstract::unload(ConfigFileType_t configFileType) {
	DebugAssertMsg(false, "아직 구현 안됨");
	// TODO: 필요시 구현
}

void DataManagerAbstract::finalizeLoader() {
	for (int i = 0; i < ConfigFileType::Max; ++i) {
		JCORE_DELETE_SAFE(m_pConfigFileLoaders[i]);
		m_bLoaded[i] = false;
	}

	m_bInitialized = false;

	_LogDebug_("설정파일에서 읽은 모든 데이터를 정리하였습니다.");
}


// ========================================================================================
// 공통 데이터 가져오기
// ========================================================================================


MonsterBaseInfo* DataManagerAbstract::getMobBaseInfo(int monsterCode) {

	const auto eType = ConfigFileType::Monster;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<MonsterBaseInfo*>(getData(eType, monsterCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;
}



ItemAvatarInfo* DataManagerAbstract::getAvatarInfo(int avatarCode) {
	const auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<ItemAvatarInfo*>(getData(eType, avatarCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;

}

ItemWeaponInfo* DataManagerAbstract::getWeaponInfo(int weaponCode) {
	const auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<ItemWeaponInfo*>(getData(eType, weaponCode));
	DebugAssertMsg(pRet, "무기 타입이 아닙니다.");
	return pRet;
}

ItemArmorInfo* DataManagerAbstract::getArmorInfo(int armorCode) {
	const auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<ItemArmorInfo*>(getData(eType, armorCode));
	DebugAssertMsg(pRet, "방어구 | 장신구 | 칭호 타입이 아닙니다.");
	return pRet;
}

ItemVisualInfo* DataManagerAbstract::getVisualInfo(int visualCode) {
	const auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<ItemVisualInfo*>(getData(eType, visualCode));
	DebugAssertMsg(pRet, "아바타 | 무기가 아닙니다.");
	return pRet;
}

CommonInfo* DataManagerAbstract::getCommonInfo(int commonConfigCode) {
	DebugAssertMsg(m_bLoaded[ConfigFileType::Common], "먼저 loadCommon()을 호출해주세요");
	return (CommonInfo*)m_pConfigFileLoaders[ConfigFileType::Common]->getData(commonConfigCode);
}

ItemOptInfo* DataManagerAbstract::getItemOptInfo(int itemOptCode) {
	const auto eType = ConfigFileType::ItemOpt;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<ItemOptInfo*>(getData(eType, itemOptCode));
	DebugAssertMsg(pRet, "아이템 옵트 타입이 아닙니다.");
	return pRet;
}

ItemOptInfo* DataManagerAbstract::getItemOptInfo(const SGString& itemOptEngName) {
	const auto eType = ConfigFileType::ItemOpt;

	if (!m_bLoaded[eType])
		load(eType);

	return ((ItemOptInfoLoader*)m_pConfigFileLoaders[eType])->getData(itemOptEngName);
}

ChannelBaseInfo* DataManagerAbstract::getChannelBaseInfo(int channelCode) {
	const auto eType = ConfigFileType::Channel;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<ChannelBaseInfo*>(getData(eType, channelCode));
	DebugAssertMsg(pRet, "채널 인포 타입이 아닙니다.");
	return pRet;
}

EnchantInfo* DataManagerAbstract::getEnchantInfo(int enchantCode) {
	const auto eType = ConfigFileType::Enchant;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<EnchantInfo*>(getData(eType, enchantCode));
	DebugAssertMsg(pRet, "인챈트 인포 타입이 아닙니다.");
	return pRet;
}

ServerProcessInfoPackage* DataManagerAbstract::getServerProcessInfoPackage(int serverCode) {
	const auto eType = ConfigFileType::Server;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<ServerProcessInfoPackage*>(getData(eType, serverCode));
	DebugAssertMsg(pRet, "서버 인포 타입이 아닙니다.");
	return pRet;
}

DatabaseInfo* DataManagerAbstract::getDatabaseInfo(int databaseCode) {
	const auto eType = ConfigFileType::Database;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<DatabaseInfo*>(getData(eType, databaseCode));
	DebugAssertMsg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}

CharCommonInfo* DataManagerAbstract::getCharCommonInfo(int charCommonCode) {
	const auto eType = ConfigFileType::Char_Common;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<CharCommonInfo*>(getData(eType, charCommonCode));
	DebugAssertMsg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}

MapInfo* DataManagerAbstract::getMapInfo(int mapCode) {
	const auto eType = ConfigFileType::Map;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<MapInfo*>(getData(eType, mapCode));
	DebugAssertMsg(pRet, "맵 인포 타입이 아닙니다.");
	return pRet;
}

MapAreaInfo* DataManagerAbstract::getMapAreaInfo(int mapCode) {
	const auto eType = ConfigFileType::Map;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pLoader = dynamic_cast<MapInfoLoader*>(m_pConfigFileLoaders[eType]);

	if (pLoader == nullptr) {
		_LogWarn_("맵 인포 로더가 없습니다.");
		return nullptr;
	}

	MapAreaInfo* pRet = pLoader->getMapAreaInfo(mapCode);
	if (pRet == nullptr) {
		_LogWarn_("%d 맵의 Area 데이터를 찾지 못했습니다.", mapCode);
		return nullptr;
	}
	return pRet;
}

MapPhysicsInfo* DataManagerAbstract::getMapPhysicsInfo(int physicsCode) {
	const auto eType = ConfigFileType::MapPhysics;

	if (!m_bLoaded[eType])
		load(eType);

	const auto pRet = dynamic_cast<MapPhysicsInfo*>(getData(eType, physicsCode));
	DebugAssertMsg(pRet, "맵 피직스 타입이 아닙니다.");
	return pRet;
}



