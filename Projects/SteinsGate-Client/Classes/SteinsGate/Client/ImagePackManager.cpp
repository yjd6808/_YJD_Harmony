/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:51:44 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ImagePackManager.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Common/SgaLoader.h>
#include <SteinsGate/Common/SgaElementInitializer.h>

#include <JCore/Threading/Thread.h>

#include <SteinsGate/Client/Global.h>



USING_NS_CC;
USING_NS_JC;

ImagePackManager::ImagePackManager() {}
ImagePackManager::~ImagePackManager() {
	for (int i = 0; i < MaxSgaFileCount_v; ++i) {
		JCORE_DELETE_SAFE(m_LoadedPackages[i]);
	}
}

void ImagePackManager::loadAllPackages() {

	SGThread loaderThread[MaxSgaParallelLoadingThreadCount_v];
	SGString imageDirPath = Path::Combine(CoreCommonInfo_v->DataPath, ImageDirName_v);
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
				m_LoadedPackages[j] = dbg_new ImagePack(package, j);
			}
		});
	}

	for (int i = 0; i < MaxSgaParallelLoadingThreadCount_v; ++i) {
		loaderThread[i].Join();
	}
	_LogInfo_("sga 파일 %d개 헤더 인덱싱 완료", m_iLoadedPackageCount);
}

void ImagePackManager::unloadPackData(int packIndex) {
	getPack(packIndex)->unload();
}

ImagePack* ImagePackManager::getPack(const SGString& packName) {
	DebugAssertMsg(m_PathToIdMap.Exist(packName), "해당 패키지가 존재하지 않습니다.");
	return m_LoadedPackages[m_PathToIdMap[packName]];
}

ImagePack* ImagePackManager::getPackUnsafe(const SGString& packName) {
	if (!m_PathToIdMap.Exist(packName))
		return nullptr;

	return m_LoadedPackages[m_PathToIdMap[packName]];
}

ImagePack* ImagePackManager::getPack(const int idx) {
	DebugAssertMsg(IsValidPackIndex(idx), "올바르지 않은 패키지 인덱스 입니다.");
	return m_LoadedPackages[idx];
}

ImagePack* ImagePackManager::getPackUnsafe(const int idx) {
	if (!IsValidPackIndex(idx)) {
		return nullptr;
	}

	return m_LoadedPackages[idx];
}

ImagePack* ImagePackManager::getAvatarPack(CharType_t charType, AvatarType_t part) {
	DebugAssertMsg(charType >= CharType::Begin && charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMsg(part >= AvatarType::Begin && part < AvatarType::Max, "아바타 타입이 올바르지 않습니다.");

	if (m_AvatarPacks[charType][part] == nullptr) {
		const SGString& sgaName = Global::Get()->getAvatarSgaName(charType, part);
		m_AvatarPacks[charType][part] = getPack(sgaName);
	}

	return m_AvatarPacks[charType][part];
}

ImagePack* ImagePackManager::getWeaponPack(WeaponType_t weaponType) {
	DebugAssertMsg(weaponType >= WeaponType::Begin && weaponType < WeaponType::Max, "무기 타입이 올바르지 않습니다.");

	if (m_WeaponPacks[weaponType] == nullptr) {
		const SGString& sgaName = Global::Get()->getWeaponSgaName(weaponType);
		m_WeaponPacks[weaponType] = getPack(sgaName);
	}

	return m_WeaponPacks[weaponType];
}

int ImagePackManager::getAvatarPackIndex(CharType_t charType, AvatarType_t avatarType) {
	return getAvatarPack(charType, avatarType)->getPackIndex();
}


int ImagePackManager::getWeaponPackIndex(WeaponType_t weaponType) {
	return getWeaponPack(weaponType)->getPackIndex();
}


int ImagePackManager::getPackIndex(const SGString& packPath) {
	DebugAssertMsg(m_PathToIdMap.Exist(packPath), "해당 패키지가 존재하지 않습니다. (2)");
	return m_PathToIdMap[packPath];
}

int ImagePackManager::getPackIndexDefault(const SGString& packPath, int defaultIndex) {
	if (!m_PathToIdMap.Exist(packPath))
		return defaultIndex;

	return m_PathToIdMap[packPath];
}

void ImagePackManager::releaseFrameTexture(int packIndex, int imgIndex, int frameIndex) {
	getPack(packIndex)->releaseFrameTexture(imgIndex, frameIndex);
}

void ImagePackManager::releaseFrameTexture(const SgaResourceIndex& sgaResourceIndex) {
	getPack(sgaResourceIndex.Un.SgaIndex)->releaseFrameTexture(
		sgaResourceIndex.Un.ImgIndex, 
		sgaResourceIndex.Un.FrameIndex);
}

void ImagePackManager::releaseAllFrameTexture() {
	for (int i = 0; i < m_iLoadedPackageCount; ++i) {
		ImagePack* pPack = m_LoadedPackages[i];
		pPack->clearCache();
	}
}

void ImagePackManager::increaseCounter() {
	m_TextureCounter.CachedCount++;
	m_TextureCounter.MaxCachedCount = Math::Max(m_TextureCounter.CachedCount, m_TextureCounter.MaxCachedCount);
}

void ImagePackManager::decreaseCounter() {
	m_TextureCounter.CachedCount--;
}

void ImagePackManager::resetCounter() {
	m_TextureCounter.CachedCount = 0;
}

void ImagePackManager::logTexture(const SGString& text, const SgaResourceIndex& resource, LoggerAbstract::Level logLevel) {
#if DebugMode
	const ImagePack* pPack = getPackUnsafe(resource.Un.SgaIndex);
	const SGString szSgaName = pPack ? pPack->getFileName() : "없음";
	const SGString szImgName = pPack ? pPack->getImgNameOrDefault(resource.Un.ImgIndex, "없음") : "없음";

	if (!Logger_v)
		return;

	if (resource.Un.FrameIndex == InvalidValue_v) {
		Logger_v->Log(logLevel, "%s / Sga: %s(%d) / Img: %s(%d)", text.Source(), szSgaName.Source(), resource.Un.SgaIndex, szImgName.Source(), resource.Un.ImgIndex);
		return;
	}

	Logger_v->Log(logLevel, "%s / Sga: %s(%d) / Img: %s(%d) / Frame: %d", text.Source(), szSgaName.Source(), resource.Un.SgaIndex, szImgName.Source(), resource.Un.ImgIndex, resource.Un.FrameIndex);
#endif
}

bool ImagePackManager::IsValidPackIndex(int idx) {
	if (idx >= 0 && idx < m_iLoadedPackageCount)
		return true;

	return false;
}

