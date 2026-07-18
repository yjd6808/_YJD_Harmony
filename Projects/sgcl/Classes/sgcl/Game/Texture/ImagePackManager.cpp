/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:51:44 PM
 * =====================
 *
 */

#include "Game/Texture/ImagePackManager.h"

#include "sg/Core/AppConfig.h"
#include "sg/Sga/SgaLoader.h"
#include "sg/Sga/SgaElementInitializer.h"


USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ImagePackManager::ImagePackManager()
{
	for (int i = 0; i < CharType::Max; ++i)
	{
		for (int j = 0; j < AvatarType::Max; ++j)
		{
			avatarPackName_[i][j] = StringUtil::Format("%s_avatar_%s.sga", CharType::Name[i], AvatarType::Name[j]);
		}
	}

	weaponPackName_[WeaponType::Automatic] = "gunner_weapon_auto.sga";
	weaponPackName_[WeaponType::Bowgun] = "gunner_weapon_bowgun.sga";
	weaponPackName_[WeaponType::HandCannon] = "gunner_weapon_hcan.sga";
	weaponPackName_[WeaponType::Musket] = "gunner_weapon_musket.sga";
	weaponPackName_[WeaponType::Revolver] = "gunner_weapon_rev.sga";
}

//////////////////////////////////////////////////////////////////////////////////////////
const jc::String& ImagePackManager::GetAvatarSgaName(int _charType, int _avatarType)
{
	jc_assert_msg(_charType >= CharType::Begin && _charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다. [1]");
	jc_assert_msg(_avatarType >= AvatarType::Begin && _avatarType < AvatarType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return avatarPackName_[_charType][_avatarType];
}

//////////////////////////////////////////////////////////////////////////////////////////
const jc::String& ImagePackManager::GetWeaponSgaName(int _weaponType)
{
	jc_assert_msg(_weaponType >= WeaponType::Begin && _weaponType < WeaponType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return weaponPackName_[_weaponType];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePackManager::~ImagePackManager()
{
	for (int i = 0; i < Const::Resource::MaxSgaFileCount; ++i)
	{
		JC_DELETE_SAFE(loadedPackages_[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::LoadAllPackages()
{
	constexpr int THREAD_COUNT = Const::Resource::MaxSgaParallelLoadingThreadCount;

	const jc::String& resDataPath = g_cAppConfig.resDataImagePath_;
	jc::Thread loaderThread[THREAD_COUNT];
	jc::Vector<jc::String> paths = Directory::Files(resDataPath, true);
	jc::Vector<jc::String> sgaPaths;

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
void ImagePackManager::UnloadPackData(int _packIndex)
{
	GetPack(_packIndex)->Unload();
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::GetPack(const jc::String& _packName)
{
	jc_assert_msg(pathToIdMap_.Exist(_packName), "해당 패키지가 존재하지 않습니다.");
	return loadedPackages_[pathToIdMap_[_packName]];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::GetPack(const char* _pPackName)
{
	jc_assert_msg(pathToIdMap_.Exist(_pPackName), "해당 패키지가 존재하지 않습니다.");
	return loadedPackages_[pathToIdMap_[_pPackName]];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::GetPackUnsafe(const jc::String& _packName)
{
	if (!pathToIdMap_.Exist(_packName))
	{
		return nullptr;
	}

	return loadedPackages_[pathToIdMap_[_packName]];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::GetPackUnsafe(const char* _pPackName)
{
	if (!pathToIdMap_.Exist(_pPackName))
	{
		return nullptr;
	}

	return loadedPackages_[pathToIdMap_[_pPackName]];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::GetPack(const int _index)
{
	jc_assert_msg(IsValidPackIndex(_index), "올바르지 않은 패키지 인덱스 입니다.");
	return loadedPackages_[_index];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::GetPackUnsafe(const int _index)
{
	if (!IsValidPackIndex(_index))
	{
		return nullptr;
	}

	return loadedPackages_[_index];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::GetAvatarPack(CharType_t _charType, AvatarType_t _avatarType)
{
	jc_assert_msg(_charType >= CharType::Begin && _charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다.");
	jc_assert_msg(_avatarType >= AvatarType::Begin && _avatarType < AvatarType::Max, "아바타 타입이 올바르지 않습니다.");

	if (avatarPacks_[_charType][_avatarType] == nullptr)
	{
		const jc::String& sgaName = g_cImagePackMgr.GetAvatarSgaName(_charType, _avatarType);
		avatarPacks_[_charType][_avatarType] = GetPack(sgaName);
	}

	return avatarPacks_[_charType][_avatarType];
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack* ImagePackManager::GetWeaponPack(WeaponType_t _weaponType)
{
	jc_assert_msg(_weaponType >= WeaponType::Begin && _weaponType < WeaponType::Max, "무기 타입이 올바르지 않습니다.");

	if (weaponPacks_[_weaponType] == nullptr)
	{
		const jc::String& sgaName = g_cImagePackMgr.GetWeaponSgaName(_weaponType);
		weaponPacks_[_weaponType] = GetPack(sgaName);
	}

	return weaponPacks_[_weaponType];
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::GetAvatarPackIndex(CharType_t _charType, AvatarType_t _avatarType)
{
	return GetAvatarPack(_charType, _avatarType)->GetPackIndex();
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::GetWeaponPackIndex(WeaponType_t _weaponType)
{
	return GetWeaponPack(_weaponType)->GetPackIndex();
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::GetPackIndex(const jc::String& _packName)
{
	jc_assert_msg(pathToIdMap_.Exist(_packName), "해당 패키지가 존재하지 않습니다. (2)");
	return pathToIdMap_[_packName];
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::GetPackIndexDefault(const char* _pPackName, int _defaultIndex /* = InvalidValue_v */)
{
	if (!pathToIdMap_.Exist(_pPackName))
	{
		return _defaultIndex;
	}

	return pathToIdMap_[_pPackName];
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::GetPackIndexDefault(const jc::String& _packName, int _defaultIndex /* = InvalidValue_v */)
{
	if (!pathToIdMap_.Exist(_packName))
	{
		return _defaultIndex;
	}

	return pathToIdMap_[_packName];
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::GetImgIndexUnsafe(int _sgaIndex, const char* _pImgName)
{
	const ImagePack* pPack = GetPackUnsafe(_sgaIndex);
	if (pPack == nullptr)
	{
		return InvalidValue_v;
	}

	return pPack->GetImgIndex(_pImgName);
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePackManager::GetImgIndexUnsafe(int _sgaIndex, const jc::String& _imgName)
{
	const ImagePack* pPack = GetPackUnsafe(_sgaIndex);
	if (pPack == nullptr)
	{
		return InvalidValue_v;
	}

	return pPack->GetImgIndex(_imgName);
}

//////////////////////////////////////////////////////////////////////////////////////////
cc::size ImagePackManager::GetTextureSize(int _packIndex, int _imgIndex, int _frameIndex)
{
	const ImagePack* pPack = GetPackUnsafe(_packIndex);
	if (pPack == nullptr)
	{
		LogTexture("이미지팩을 찾지 못함.", { _packIndex, _imgIndex, _frameIndex }, LoggerAbstract::eWarn);
		return {};
	}

	SgaSpriteAbstractPtr pSprite = pPack->GetSpriteUnsafe(_imgIndex, _frameIndex);
	if (pSprite == nullptr)
	{
		LogTexture("스프라이트를 찾지 못함.", { _packIndex, _imgIndex, _frameIndex }, LoggerAbstract::eWarn);
		return {};
	}

	SgaSpriteRect rect = pSprite->GetRect();
	return { rect.GetWidthF(), rect.GetHeightF() };
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::ReleaseFrameTexture(int _packIndex, int _imgIndex, int _frameIndex)
{
	GetPack(_packIndex)->ReleaseFrameTexture(_imgIndex, _frameIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::ReleaseFrameTexture(const SgaResourceIndex& _sgaResourceIndex)
{
	GetPack(_sgaResourceIndex.un_.sgaIndex_)->ReleaseFrameTexture(
		_sgaResourceIndex.un_.imgIndex_,
		_sgaResourceIndex.un_.frameIndex_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::ReleaseAllFrameTexture()
{
	for (int i = 0; i < loadedPackageCount_; ++i)
	{
		ImagePack* pPack = loadedPackages_[i];
		pPack->ClearCache();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::IncreaseCounter()
{
	textureCounter_.cachedCount_++;
	textureCounter_.maxCachedCount_ = Math::Max(textureCounter_.cachedCount_, textureCounter_.maxCachedCount_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::DecreaseCounter()
{
	textureCounter_.cachedCount_--;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::ResetCounter()
{
	textureCounter_.cachedCount_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePackManager::LogTexture(const jc::String& _text, const SgaResourceIndex& _resource,
                                  LoggerAbstract::Level _logLevel)
{
#if _DEBUG
	const ImagePack* pPack = GetPackUnsafe(_resource.un_.sgaIndex_);
	const jc::String sgaName = pPack ? pPack->GetFileName() : "없음";
	const jc::String imgName = pPack ? pPack->GetImgNameOrDefault(_resource.un_.imgIndex_, "없음") : "없음";

	if (!Logger_v)
	{
		return;
	}

	if (_resource.un_.frameIndex_ == InvalidValue_v)
	{
		Logger_v->Log(_logLevel, "%s / Sga: %s(%d) / Img: %s(%d)", _text.Source(), sgaName.Source(),
		              _resource.un_.sgaIndex_, imgName.Source(), _resource.un_.imgIndex_);
		return;
	}

	Logger_v->Log(_logLevel, "%s / Sga: %s(%d) / Img: %s(%d) / Frame: %d", _text.Source(), sgaName.Source(),
	              _resource.un_.sgaIndex_, imgName.Source(), _resource.un_.imgIndex_, _resource.un_.frameIndex_);
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
