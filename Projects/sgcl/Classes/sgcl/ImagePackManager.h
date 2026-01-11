/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:46:52 PM
 * =====================
 *
 */

#pragma once

#include <sgcl/ImagePack.h>

class ImagePackManager final : public jc::SingletonPointer<ImagePackManager>
{
private:
	friend class TSingleton;
	ImagePackManager();
	~ImagePackManager();

public:
	void LoadAllPackages();
	void UnloadPackData(int _packIndex);

	const jc::String& GetAvatarSgaName(int _charType, int _avatarType);
	const jc::String& GetWeaponSgaName(int _weaponType);

	ImagePack* GetPack(const jc::String& _packName);
	ImagePack* GetPack(const char* _pPackName);
	ImagePack* GetPackUnsafe(const jc::String& _packName);
	ImagePack* GetPackUnsafe(const char* _pPackName);
	ImagePack* GetPack(const int _index);
	ImagePack* GetPackUnsafe(const int _index);
	ImagePack* GetAvatarPack(CharType_t _charType, AvatarType_t _avatarType);
	ImagePack* GetWeaponPack(WeaponType_t _weaponType);

	int GetAvatarPackIndex(CharType_t _charType, AvatarType_t _avatarType);
	int GetWeaponPackIndex(WeaponType_t _weaponType);
	int GetPackIndex(const jc::String& _packName);
	int GetPackIndexDefault(const char* _pPackName, int _defaultIndex = InvalidValue_v);
	int GetPackIndexDefault(const jc::String& _packName, int _defaultIndex = InvalidValue_v);

	int GetImgIndexUnsafe(int _sgaIndex, const char* _pImgName);
	int GetImgIndexUnsafe(int _sgaIndex, const jc::String& _imgName);

	cc::size GetTextureSize(int _packIndex, int _imgIndex, int _frameIndex);

	void ReleaseFrameTexture(int _packIndex, int _imgIndex, int _frameIndex);
	void ReleaseFrameTexture(const SgaResourceIndex& _sgaResourceIndex);
	void ReleaseAllFrameTexture();

	void IncreaseCounter();
	void DecreaseCounter();
	void ResetCounter();
	CachedTextureCounter& GetCounter() { return textureCounter_; }

	void LogTexture(const jc::String& _text, const SgaResourceIndex& _resource, jc::LoggerAbstract::Level _logLevel);

private:
	bool IsValidPackIndex(int _index);

	jc::HashMap<jc::String, int> pathToIdMap_;
	ImagePack* loadedPackages_[Const::Resource::MaxSgaFileCount]{};
	ImagePack* avatarPacks_[CharType::Max][AvatarType::Max]{};
	ImagePack* weaponPacks_[WeaponType::Max]{};

	jc::String avatarPackName_[CharType::Max][AvatarType::Max];
	jc::String weaponPackName_[WeaponType::Max];

	CachedTextureCounter textureCounter_;
	int loadedPackageCount_{};
};

JC_DECL_SINGLETON_VAR(ImagePackManager);
#define g_cImagePackMgr JC_DECL_SINGLETON_BODY(ImagePackManager)
