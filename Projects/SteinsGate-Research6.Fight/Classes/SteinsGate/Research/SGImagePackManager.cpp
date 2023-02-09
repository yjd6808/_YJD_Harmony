/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:51:44 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGImagePackManager.h"

#include <SteinsGate/Common/Core/Npk/NpkLoader.h>
#include <SteinsGate/Common/Core/Npk/NpkElementInitializer.h>

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/Threading/Thread.h>

#include <SteinsGate/Research/SGGlobal.h>


USING_NS_CC;
USING_NS_JC;

SGImagePackManager::SGImagePackManager() {}
SGImagePackManager::~SGImagePackManager() {
	for (int i = 0; i < MaxNpkFileCount_v; ++i) {
		DeleteSafe(m_LoadedPackages[i]);
	}
}

void SGImagePackManager::loadAllPackages() {

	NpkElementInitializer::Initialize();

	JCore::Thread loaderThread[MaxNpkParallelLoadingThreadCount_v];
	SGVector<SGString> paths = Directory::Files(DataDirectory_v, false);

	for (int i = 0; i < paths.Size(); ++i) {
		m_PathToIdMap.Insert(Path::FileName(paths[i]), i);
	}

	m_iLoadedPackageCount = paths.Size();
	
	// 8개씩 병렬 로딩 진행
	for (int i = 0; i < MaxNpkParallelLoadingThreadCount_v; ++i) {
		loaderThread[i].Start([i, this, &paths](void*) {
			for (int j = i; j < m_iLoadedPackageCount; j += MaxNpkParallelLoadingThreadCount_v) {
				SGString szFileName =  Path::FileName(paths[j]);
				NpkPackagePtr package = NpkLoader::LoadIndexOnly(
					Path::Combine(DataDirectory_v, szFileName)
				);
				m_LoadedPackages[j] = new SGImagePack(package, j);
				SafeConsole::WriteLine("%d %s 로딩완료", j, szFileName.Source());
			}
		});
	}

	

	for (int i = 0; i < MaxNpkParallelLoadingThreadCount_v; ++i) {
		loaderThread[i].Join();
	}
	SGConsole::WriteLine("NPK파일들 인덱스 로딩 완료");
}

SGImagePack* SGImagePackManager::getPack(const SGString& packName) {
	DebugAssertMsg(m_PathToIdMap.Exist(packName), "해당 패키지가 존재하지 않습니다.");
	return m_LoadedPackages[m_PathToIdMap[packName]];
}
SGImagePack* SGImagePackManager::getPack(const int idx) {
	DebugAssertMsg(idx >= 0 && idx < m_iLoadedPackageCount, "올바르지 않은 패키지 인덱스 입니다.");
	return m_LoadedPackages[idx];
}

SGImagePack* SGImagePackManager::getAvatarPack(CharType_t charType, AvatarType_t part) {
	DebugAssertMsg(charType >= CharType::Begin && charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMsg(part >= AvatarType::Begin && part < AvatarType::Max, "아바타 타입이 올바르지 않습니다.");

	if (m_AvatarPacks[charType][part] == nullptr) {
		const SGString& npkName = SGGlobal::get()->getAvatarNpkName(charType, part);
		m_AvatarPacks[charType][part] = getPack(npkName);
	}

	return m_AvatarPacks[charType][part];
}

SGImagePack* SGImagePackManager::getWeaponPack(CharType_t charType, WeaponType_t weaponType) {
	DebugAssertMsg(charType >= CharType::Begin && charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMsg(weaponType >= WeaponType::Begin && weaponType < WeaponType::Max, "무기 타입이 올바르지 않습니다.");

	if (m_WeaponPacks[charType][weaponType] == nullptr) {
		const SGString& npkName = SGGlobal::get()->getWeaponNpkName(charType, weaponType);
		m_WeaponPacks[charType][weaponType] = getPack(npkName);
	}

	return m_WeaponPacks[charType][weaponType];
}

int SGImagePackManager::getAvatarPackIndex(CharType_t charType, AvatarType_t avatarType) {
	return getAvatarPack(charType, avatarType)->getPackIndex();
}

int SGImagePackManager::getWeaponPackIndex(CharType_t charType, WeaponType_t weaponType) {
	return getWeaponPack(charType, weaponType)->getPackIndex();
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

