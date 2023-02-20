/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 6:40:01 PM
 * =====================
 *
 */


#include "Core.h"
#include "SGDataManagerAbstract.h"

SGDataManagerAbstract::SGDataManagerAbstract()
	: m_pConfigFileLoaders{}
	, m_bLoaded{}
{}

SGDataManagerAbstract::~SGDataManagerAbstract() {
	finalizeLoader();
}

void SGDataManagerAbstract::loadAll() {
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

ConfigDataAbstract* SGDataManagerAbstract::getData(ConfigFileType_t configFileType, int code) {
	DebugAssertMsg(configFileType >= ConfigFileType::Begin && configFileType <= ConfigFileType::End
		, "올바르지 않은 ConfigFileType 입니다.");
	return m_pConfigFileLoaders[configFileType]->getData(code);
}

void SGDataManagerAbstract::load(ConfigFileType_t configFileType) {
	ConfigFileLoaderAbstract* pLoader = m_pConfigFileLoaders[configFileType];
	DebugAssertMsg(pLoader != nullptr, "%s 파일 로더가 아직 생성되어있지 않습니다.", ConfigFileType::FileName[configFileType]);
	m_bLoaded[configFileType] = pLoader->load();
}

void SGDataManagerAbstract::unload(ConfigFileType_t configFileType) {
	DebugAssertMsg(false, "아직 구현 안됨");
	// TODO: 필요시 구현
}

void SGDataManagerAbstract::finalizeLoader() {
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


MobBaseInfo* SGDataManagerAbstract::getMobBaseInfo(int monsterCode) {

	auto eType = ConfigFileType::Monster;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<MobBaseInfo*>(getData(eType, monsterCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;
}



ItemAvatarInfo* SGDataManagerAbstract::getAvatarInfo(int avatarCode) {
	auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ItemAvatarInfo*>(getData(eType, avatarCode));
	DebugAssertMsg(pRet, "아바타 타입이 아닙니다.");
	return pRet;

}

ItemWeaponInfo* SGDataManagerAbstract::getWeaponInfo(int weaponCode) {
	auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ItemWeaponInfo*>(getData(eType, weaponCode));
	DebugAssertMsg(pRet, "무기 타입이 아닙니다.");
	return pRet;
}

ItemArmorInfo* SGDataManagerAbstract::getArmorInfo(int armorCode) {
	auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ItemArmorInfo*>(getData(eType, armorCode));
	DebugAssertMsg(pRet, "방어구 | 장신구 | 칭호 타입이 아닙니다.");
	return pRet;
}

ItemVisualInfo* SGDataManagerAbstract::getVisualInfo(int visualCode) {
	auto eType = ConfigFileType::Item;

	if (!m_bLoaded[eType])
		load(eType);

	auto pRet = dynamic_cast<ItemVisualInfo*>(getData(eType, visualCode));
	DebugAssertMsg(pRet, "아바타 | 무기가 아닙니다.");
	return pRet;
}




