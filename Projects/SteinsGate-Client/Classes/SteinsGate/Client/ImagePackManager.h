/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:46:52 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/ImagePack.h>

class ImagePackManager final : public JCore::SingletonPointer<ImagePackManager>
{
private:
	friend class TSingleton;
	ImagePackManager();
	~ImagePackManager();

public:
	void LoadAllPackages();
	void UnloadPackData(int _packIndex);

	ImagePack* GetPack(const SGString& _packName);
	ImagePack* GetPack(const char* _pPackName);
	ImagePack* GetPackUnsafe(const SGString& _packName);
	ImagePack* GetPackUnsafe(const char* _pPackName);
	ImagePack* GetPack(const int _index);
	ImagePack* GetPackUnsafe(const int _index);
	ImagePack* GetAvatarPack(CharType_t _charType, AvatarType_t _avatarType);
	ImagePack* GetWeaponPack(WeaponType_t _weaponType);

	int GetAvatarPackIndex(CharType_t _charType, AvatarType_t _avatarType);
	int GetWeaponPackIndex(WeaponType_t _weaponType);
	int GetPackIndex(const SGString& _packName);
	int GetPackIndexDefault(const char* _pPackName, int _defaultIndex = InvalidValue_v);
	int GetPackIndexDefault(const SGString& _packName, int _defaultIndex = InvalidValue_v);

	int GetImgIndexUnsafe(int _sgaIndex, const char* _pImgName);
	int GetImgIndexUnsafe(int _sgaIndex, const SGString& _imgName);

	SGSize GetTextureSize(int _packIndex, int _imgIndex, int _frameIndex);

	void ReleaseFrameTexture(int _packIndex, int _imgIndex, int _frameIndex);
	void ReleaseFrameTexture(const SgaResourceIndex& _sgaResourceIndex);
	void ReleaseAllFrameTexture();

	void IncreaseCounter();
	void DecreaseCounter();
	void ResetCounter();
	CachedTextureCounter& GetCounter() { return textureCounter_; }

	void LogTexture(const SGString& _text, const SgaResourceIndex& _resource, JCore::LoggerAbstract::Level _logLevel);

private:
	bool IsValidPackIndex(int _index);

	SGHashMap<SGString, int> pathToIdMap_;
	ImagePack* loadedPackages_[Const::Resource::MaxSgaFileCount]{};
	ImagePack* avatarPacks_[CharType::Max][AvatarType::Max]{};
	ImagePack* weaponPacks_[WeaponType::Max]{};
	CachedTextureCounter textureCounter_;
	int loadedPackageCount_{};
};
