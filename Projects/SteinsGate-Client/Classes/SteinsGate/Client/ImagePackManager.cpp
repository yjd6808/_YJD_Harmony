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

//////////////////////////////////////////////////////////////////////////////////////////
ImagePackManager::ImagePackManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePackManager::~ImagePackManager()
{
	for (int i = 0; i < Const::Resource::MaxSgaFileCount; ++i)
	{
		JCORE_DELETE_SAFE(loadedPackages_[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::loadAllPackages()
{
	constexpr int THREAD_COUNT = Const::Resource::MaxSgaParallelLoadingThreadCount;

	SGThread loaderThread[THREAD_COUNT];
	SGString imageDirPath = Path::Combine(Core::CommonInfo->dataPath_, Const::Resource::ImageDirName);
	SGVector<SGString> paths = Directory::Files(imageDirPath, false);
	SGVector<SGString> sgaPaths;

	for (int i = 0; i < paths.Size(); ++i)
	{
		if (paths[i].EndWith(".sga"))
		{
			sgaPaths.PushBack(paths[i]);
		}
	}

	for (int i = 0; i < sgaPaths.Size(); ++i)
	{
		pathToIdMap_.Insert(Path::FileName(sgaPaths[i]), i);
	}

	loadedPackageCount_ = sgaPaths.Size();

	// 8개씩 병렬 로딩 진행
	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		loaderThread[i].Start([i, this, &sgaPaths](void*)
		{
			for (int j = i; j < loadedPackageCount_; j += THREAD_COUNT)
			{
				SgaPackagePtr pPackage = SgaLoader::LoadHeaderOnly(sgaPaths[j]);
				loadedPackages_[j] = dbg_new ImagePack(pPackage, j);
			}
		});
	}

	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		loaderThread[i].Join();
	}

	_LogInfo_("sga 파일 %d개 헤더 인덱싱 완료", loadedPackageCount_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::unloadPackData(int _packIndex)
{
	getPack(_packIndex)->unload();
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::getPack(const SGString& _packName)
{
	DebugAssertMsg(pathToIdMap_.Exist(_packName), "해당 패키지가 존재하지 않습니다.");
	return loadedPackages_[pathToIdMap_[_packName]];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::getPack(const char* _pPackName)
{
	DebugAssertMsg(pathToIdMap_.Exist(_pPackName), "해당 패키지가 존재하지 않습니다.");
	return loadedPackages_[pathToIdMap_[_pPackName]];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::getPackUnsafe(const SGString& _packName)
{
	if (!pathToIdMap_.Exist(_packName))
	{
		return nullptr;
	}

	return loadedPackages_[pathToIdMap_[_packName]];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::getPackUnsafe(const char* _pPackName)
{
	if (!pathToIdMap_.Exist(_pPackName))
	{
		return nullptr;
	}

	return loadedPackages_[pathToIdMap_[_pPackName]];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::getPack(const int _index)
{
	DebugAssertMsg(IsValidPackIndex(_index), "올바르지 않은 패키지 인덱스 입니다.");
	return loadedPackages_[_index];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::getPackUnsafe(const int _index)
{
	if (!IsValidPackIndex(_index))
	{
		return nullptr;
	}

	return loadedPackages_[_index];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::getAvatarPack(CharType_t _charType, AvatarType_t _avatarType)
{
	DebugAssertMsg(_charType >= CharType::Begin && _charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMsg(_avatarType >= AvatarType::Begin && _avatarType < AvatarType::Max, "아바타 타입이 올바르지 않습니다.");

	if (avatarPacks_[_charType][_avatarType] == nullptr)
	{
		const SGString& sgaName = Global::Get()->getAvatarSgaName(_charType, _avatarType);
		avatarPacks_[_charType][_avatarType] = getPack(sgaName);
	}

	return avatarPacks_[_charType][_avatarType];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::getWeaponPack(WeaponType_t _weaponType)
{
	DebugAssertMsg(_weaponType >= WeaponType::Begin && _weaponType < WeaponType::Max, "무기 타입이 올바르지 않습니다.");

	if (weaponPacks_[_weaponType] == nullptr)
	{
		const SGString& sgaName = Global::Get()->getWeaponSgaName(_weaponType);
		weaponPacks_[_weaponType] = getPack(sgaName);
	}

	return weaponPacks_[_weaponType];
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::getAvatarPackIndex(CharType_t _charType, AvatarType_t _avatarType)
{
	return getAvatarPack(_charType, _avatarType)->getPackIndex();
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::getWeaponPackIndex(WeaponType_t _weaponType)
{
	return getWeaponPack(_weaponType)->getPackIndex();
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::getPackIndex(const SGString& _packName)
{
	DebugAssertMsg(pathToIdMap_.Exist(_packName), "해당 패키지가 존재하지 않습니다. (2)");
	return pathToIdMap_[_packName];
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::getPackIndexDefault(const char* _pPackName, int _defaultIndex /* = InvalidValue_v */)
{
	if (!pathToIdMap_.Exist(_pPackName))
	{
		return _defaultIndex;
	}

	return pathToIdMap_[_pPackName];
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::getPackIndexDefault(const SGString& _packName, int _defaultIndex /* = InvalidValue_v */)
{
	if (!pathToIdMap_.Exist(_packName))
	{
		return _defaultIndex;
	}

	return pathToIdMap_[_packName];
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::getImgIndexUnsafe(int _sgaIndex, const char* _pImgName)
{
	const ImagePack* pPack = getPackUnsafe(_sgaIndex);
	if (pPack == nullptr)
	{
		return InvalidValue_v;
	}

	return pPack->getImgIndex(_pImgName);
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::getImgIndexUnsafe(int _sgaIndex, const SGString& _imgName)
{
	const ImagePack* pPack = getPackUnsafe(_sgaIndex);
	if (pPack == nullptr)
	{
		return InvalidValue_v;
	}

	return pPack->getImgIndex(_imgName);
}

//////////////////////////////////////////////////////////////////////////////////////////
SGSize ImagePackManager::getTextureSize(int _packIndex, int _imgIndex, int _frameIndex)
{
	const ImagePack* pPack = getPackUnsafe(_packIndex);
	if (pPack == nullptr)
	{
		logTexture("이미지팩을 찾지 못함.", { _packIndex, _imgIndex, _frameIndex }, LoggerAbstract::eWarn);
		return {};
	}

	SgaSpriteAbstractPtr pSprite = pPack->getSpriteUnsafe(_imgIndex, _frameIndex);
	if (pSprite == nullptr)
	{
		logTexture("스프라이트를 찾지 못함.", { _packIndex, _imgIndex, _frameIndex }, LoggerAbstract::eWarn);
		return {};
	}

	SgaSpriteRect rect = pSprite->GetRect();
	return { rect.GetWidthF(), rect.GetHeightF() };
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::releaseFrameTexture(int _packIndex, int _imgIndex, int _frameIndex)
{
	getPack(_packIndex)->releaseFrameTexture(_imgIndex, _frameIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::releaseFrameTexture(const SgaResourceIndex& _sgaResourceIndex)
{
	getPack(_sgaResourceIndex.Un.SgaIndex)->releaseFrameTexture(
		_sgaResourceIndex.Un.ImgIndex,
		_sgaResourceIndex.Un.FrameIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::releaseAllFrameTexture()
{
	for (int i = 0; i < loadedPackageCount_; ++i)
	{
		ImagePack* pPack = loadedPackages_[i];
		pPack->clearCache();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::increaseCounter()
{
	textureCounter_.CachedCount++;
	textureCounter_.MaxCachedCount = Math::Max(textureCounter_.CachedCount, textureCounter_.MaxCachedCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::decreaseCounter()
{
	textureCounter_.CachedCount--;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::resetCounter()
{
	textureCounter_.CachedCount = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::logTexture(const SGString& _text, const SgaResourceIndex& _resource,
                                  LoggerAbstract::Level _logLevel)
{
#if DebugMode
	const ImagePack* pPack = getPackUnsafe(_resource.Un.SgaIndex);
	const SGString sgaName = pPack ? pPack->getFileName() : "없음";
	const SGString imgName = pPack ? pPack->getImgNameOrDefault(_resource.Un.ImgIndex, "없음") : "없음";

	if (!Logger_v)
	{
		return;
	}

	if (_resource.Un.FrameIndex == InvalidValue_v)
	{
		Logger_v->Log(_logLevel, "%s / Sga: %s(%d) / Img: %s(%d)", _text.Source(), sgaName.Source(),
		              _resource.Un.SgaIndex, imgName.Source(), _resource.Un.ImgIndex);
		return;
	}

	Logger_v->Log(_logLevel, "%s / Sga: %s(%d) / Img: %s(%d) / Frame: %d", _text.Source(), sgaName.Source(),
	              _resource.Un.SgaIndex, imgName.Source(), _resource.Un.ImgIndex, _resource.Un.FrameIndex);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ImagePackManager::IsValidPackIndex(int _index)
{
	if (_index >= 0 && _index < loadedPackageCount_)
	{
		return true;
	}

	return false;
}
