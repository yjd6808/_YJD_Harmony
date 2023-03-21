/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:51:44 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGImagePackManager.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Common/SgaLoader.h>
#include <SteinsGate/Common/SgaElementInitializer.h>

#include <JCore/Threading/Thread.h>

#include <SteinsGate/Client/SGGlobal.h>



USING_NS_CC;
USING_NS_JC;

SGImagePackManager::SGImagePackManager() {}
SGImagePackManager::~SGImagePackManager() {
	for (int i = 0; i < MaxSgaFileCount_v; ++i) {
		DeleteSafe(m_LoadedPackages[i]);
	}
}

void SGImagePackManager::loadAllPackages() {

	SGThread loaderThread[MaxSgaParallelLoadingThreadCount_v];
	SGString imageDirPath = Path::Combine(CoreCommon_v->DataPath, ImageDirName_v);
	SGVector<SGString> paths = Directory::Files(imageDirPath, false);
	SGVector<SGString> sgaPaths;

	for (int i = 0; i < paths.Size(); ++i) {
		if (paths[i].EndWith(".sga")) {
			sgaPaths.PushBack(paths[i]);
		}
	}

	for (int i = 0; i < sgaPaths.Size(); i++) {
		m_PathToIdMap.Insert(Path::FileName(sgaPaths[i]), i);
	}

	m_iLoadedPackageCount = sgaPaths.Size();

	// 8개씩 병렬 로딩 진행
	for (int i = 0; i < MaxSgaParallelLoadingThreadCount_v; ++i) {
		loaderThread[i].Start([i, this, &sgaPaths](void*) {
			for (int j = i; j < m_iLoadedPackageCount; j += MaxSgaParallelLoadingThreadCount_v) {
				SgaPackagePtr package = SgaLoader::LoadHeaderOnly(sgaPaths[j]);
				m_LoadedPackages[j] = dbg_new SGImagePack(package, j);
			}
		});
	}

	for (int i = 0; i < MaxSgaParallelLoadingThreadCount_v; ++i) {
		loaderThread[i].Join();
	}
	_LogInfo_("sga 파일 %d개 헤더 인덱싱 완료", m_iLoadedPackageCount);
}

void SGImagePackManager::unloadPackData(int packIndex) {
	getPack(packIndex)->unload();
}

SGImagePack* SGImagePackManager::getPack(const SGString& packName) {
	DebugAssertMsg(m_PathToIdMap.Exist(packName), "해당 패키지가 존재하지 않습니다.");
	return m_LoadedPackages[m_PathToIdMap[packName]];
}

SGImagePack* SGImagePackManager::getPackUnsafe(const SGString& packName) {
	if (!m_PathToIdMap.Exist(packName))
		return nullptr;

	return m_LoadedPackages[m_PathToIdMap[packName]];
}

SGImagePack* SGImagePackManager::getPack(const int idx) {
	DebugAssertMsg(IsValidPackIndex(idx), "올바르지 않은 패키지 인덱스 입니다.");
	return m_LoadedPackages[idx];
}

SGImagePack* SGImagePackManager::getPackUnsafe(const int idx) {
	if (!IsValidPackIndex(idx)) {
		return nullptr;
	}

	return m_LoadedPackages[idx];
}

SGImagePack* SGImagePackManager::getAvatarPack(CharType_t charType, AvatarType_t part) {
	DebugAssertMsg(charType >= CharType::Begin && charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMsg(part >= AvatarType::Begin && part < AvatarType::Max, "아바타 타입이 올바르지 않습니다.");

	if (m_AvatarPacks[charType][part] == nullptr) {
		const SGString& sgaName = SGGlobal::get()->getAvatarSgaName(charType, part);
		m_AvatarPacks[charType][part] = getPack(sgaName);
	}

	return m_AvatarPacks[charType][part];
}

SGImagePack* SGImagePackManager::getWeaponPack(WeaponType_t weaponType) {
	DebugAssertMsg(weaponType >= WeaponType::Begin && weaponType < WeaponType::Max, "무기 타입이 올바르지 않습니다.");

	if (m_WeaponPacks[weaponType] == nullptr) {
		const SGString& sgaName = SGGlobal::get()->getWeaponSgaName(weaponType);
		m_WeaponPacks[weaponType] = getPack(sgaName);
	}

	return m_WeaponPacks[weaponType];
}

int SGImagePackManager::getAvatarPackIndex(CharType_t charType, AvatarType_t avatarType) {
	return getAvatarPack(charType, avatarType)->getPackIndex();
}


int SGImagePackManager::getWeaponPackIndex(WeaponType_t weaponType) {
	return getWeaponPack(weaponType)->getPackIndex();
}


int SGImagePackManager::getPackIndex(const SGString& packPath) {
	DebugAssertMsg(m_PathToIdMap.Exist(packPath), "해당 패키지가 존재하지 않습니다. (2)");
	return m_PathToIdMap[packPath];
}

int SGImagePackManager::getPackIndexDefault(const SGString& packPath, int defaultIndex) {
	if (!m_PathToIdMap.Exist(packPath))
		return defaultIndex;

	return m_PathToIdMap[packPath];
}

void SGImagePackManager::releaseFrameTexture(int packIndex, int imgIndex, int frameIndex) {
	getPack(packIndex)->releaseFrameTexture(imgIndex, frameIndex);
}

void SGImagePackManager::releaseFrameTexture(const SgaResourceIndex& sgaResourceIndex) {
	getPack(sgaResourceIndex.Un.SgaIndex)->releaseFrameTexture(
		sgaResourceIndex.Un.ImgIndex, 
		sgaResourceIndex.Un.FrameIndex);
}

bool SGImagePackManager::IsValidPackIndex(int idx) {
	if (idx >= 0 && idx < m_iLoadedPackageCount)
		return true;

	return false;
}

