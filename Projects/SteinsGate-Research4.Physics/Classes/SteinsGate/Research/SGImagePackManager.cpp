/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:51:44 PM
 * =====================
 *
 */


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
	SGConsole::WriteLine("모든 NPK 헤더만 로딩 완료");
}

SGImagePack* SGImagePackManager::getPack(const SGString& packPath) {
	DebugAssertMessage(m_PathToIdMap.Exist(packPath), "해당 패키지가 존재하지 않습니다.");
	return m_LoadedPackages[m_PathToIdMap[packPath]];
}
SGImagePack* SGImagePackManager::getPack(const int idx) {
	DebugAssertMessage(idx >= 0 && idx < m_iLoadedPackageCount, "올바르지 않은 패키지 인덱스 입니다.");
	return m_LoadedPackages[idx];
}

SGImagePack* SGImagePackManager::getAvatarPack(CharacterType_t characterType, AvatarType_t part) {
	DebugAssertMessage(characterType >= CharacterType::Begin && characterType >= CharacterType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMessage(part >= AvatarType::Begin && part < AvatarType::Max, "아바타 타입이 올바르지 않습니다.");

	if (m_AvatarPacks[characterType][part] == nullptr) {
		const SGString& npkName = SGGlobal::getInstance()->getAvatarNpkName(characterType, part);
		m_AvatarPacks[characterType][part] = getPack(npkName);
	}

	return m_AvatarPacks[characterType][part];
}

SGImagePack* SGImagePackManager::getWeaponPack(CharacterType_t characterType, WeaponType_t weaponType) {
	DebugAssertMessage(characterType >= CharacterType::Begin && characterType >= CharacterType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMessage(weaponType >= WeaponType::Begin && weaponType < WeaponType::Max, "무기 타입이 올바르지 않습니다.");

	if (m_WeaponPacks[characterType][weaponType] == nullptr) {
		const SGString& npkName = SGGlobal::getInstance()->getWeaponNpkName(characterType, weaponType);
		m_WeaponPacks[characterType][weaponType] = getPack(npkName);
	}

	return m_WeaponPacks[characterType][weaponType];
}

int SGImagePackManager::getAvatarPackIndex(CharacterType_t characterType, AvatarType_t avatarType) {
	return getAvatarPack(characterType, avatarType)->getPackIndex();
}

int SGImagePackManager::getWeaponPackIndex(CharacterType_t characterType, WeaponType_t weaponType) {
	return getWeaponPack(characterType, weaponType)->getPackIndex();
}


int SGImagePackManager::getPackIndex(const SGString& packPath) {
	DebugAssertMessage(m_PathToIdMap.Exist(packPath), "해당 패키지가 존재하지 않습니다. (2)");
	return m_PathToIdMap[packPath];
}

