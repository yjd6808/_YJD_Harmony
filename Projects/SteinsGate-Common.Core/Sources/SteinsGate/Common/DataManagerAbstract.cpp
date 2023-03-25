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
#include <SteinsGate/Common/ChannelInfoLoader.h>
#include <SteinsGate/Common/EnchantInfoLoader.h>
#include <SteinsGate/Common/ServerInfoLoader.h>

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
		DeleteSafe(m_pConfigFileLoaders[i]);
		m_bLoaded[i] = false;
	}

	m_bInitialized = false;

	_LogDebug_("설정파일에서 읽은 모든 데이터를 정리하였습니다.");
}


// ========================================================================================
// 공통 데이터 가져오기
// ========================================================================================


MobBaseInfo* DataManagerAbstract::getMobBaseInfo(int monsterCode) {

	auto eType = ConfigFileType::Monster;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<MobBaseInfo*>(getData(eType, monsterCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;
}



ItemAvatarInfo* DataManagerAbstract::getAvatarInfo(int avatarCode) {
	auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ItemAvatarInfo*>(getData(eType, avatarCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;

}

ItemWeaponInfo* DataManagerAbstract::getWeaponInfo(int weaponCode) {
	auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ItemWeaponInfo*>(getData(eType, weaponCode));
	DebugAssertMsg(pRet, "무기 타입이 아닙니다.");
	return pRet;
}

ItemArmorInfo* DataManagerAbstract::getArmorInfo(int armorCode) {
	auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ItemArmorInfo*>(getData(eType, armorCode));
	DebugAssertMsg(pRet, "방어구 | 장신구 | 칭호 타입이 아닙니다.");
	return pRet;
}

ItemVisualInfo* DataManagerAbstract::getVisualInfo(int visualCode) {
	auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ItemVisualInfo*>(getData(eType, visualCode));
	DebugAssertMsg(pRet, "아바타 | 무기가 아닙니다.");
	return pRet;
}

CommonInfo* DataManagerAbstract::getCommonInfo(int commonConfigCode) {
	DebugAssertMsg(m_bLoaded[ConfigFileType::Common], "먼저 loadCommon()을 호출해주세요");
	return (CommonInfo*)m_pConfigFileLoaders[ConfigFileType::Common]->getData(commonConfigCode);
}

ItemOptInfo* DataManagerAbstract::getItemOptInfo(int itemOptCode) {
	auto eType = ConfigFileType::ItemOpt;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ItemOptInfo*>(getData(eType, itemOptCode));
	DebugAssertMsg(pRet, "아이템 옵트 타입이 아닙니다.");
	return pRet;
}

ItemOptInfo* DataManagerAbstract::getItemOptInfo(const SGString& itemOptEngName) {
	auto eType = ConfigFileType::ItemOpt;

	if (!m_bLoaded[eType])
		load(eType);

	return ((ItemOptInfoLoader*)m_pConfigFileLoaders[eType])->getData(itemOptEngName);
}

ChannelInfo* DataManagerAbstract::getChannelInfo(int channelCode) {
	auto eType = ConfigFileType::Channel;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ChannelInfo*>(getData(eType, channelCode));
	DebugAssertMsg(pRet, "채널 인포 타입이 아닙니다.");
	return pRet;
}

EnchantInfo* DataManagerAbstract::getEnchantInfo(int enchantCode) {
	auto eType = ConfigFileType::Enchant;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<EnchantInfo*>(getData(eType, enchantCode));
	DebugAssertMsg(pRet, "인챈트 인포 타입이 아닙니다.");
	return pRet;
}

ServerInfo* DataManagerAbstract::getServerInfo(int serverCode) {
	auto eType = ConfigFileType::Server;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ServerInfo*>(getData(eType, serverCode));
	DebugAssertMsg(pRet, "서버 인포 타입이 아닙니다.");
	return pRet;
}

DatabaseInfo* DataManagerAbstract::getDatabaseInfo(int databaseCode) {
	auto eType = ConfigFileType::Database;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<DatabaseInfo*>(getData(eType, databaseCode));
	DebugAssertMsg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}

SGCharCommonInfo* DataManagerAbstract::getCharCommonInfo(int charCommonCode) {
	auto eType = ConfigFileType::Char_Common;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<SGCharCommonInfo*>(getData(eType, charCommonCode));
	DebugAssertMsg(pRet, "데이터베이스 인포 타입이 아닙니다.");
	return pRet;
}



