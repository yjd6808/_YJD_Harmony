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
	void loadAllPackages();
	void unloadPackData(int _packIndex);

	ImagePack* getPack(const SGString& _packName);
	ImagePack* getPack(const char* _pPackName);
	ImagePack* getPackUnsafe(const SGString& _packName);
	ImagePack* getPackUnsafe(const char* _pPackName);
	ImagePack* getPack(const int _index);
	ImagePack* getPackUnsafe(const int _index);
	ImagePack* getAvatarPack(CharType_t _charType, AvatarType_t _avatarType);
	ImagePack* getWeaponPack(WeaponType_t _weaponType);

	int getAvatarPackIndex(CharType_t _charType, AvatarType_t _avatarType);
	int getWeaponPackIndex(WeaponType_t _weaponType);
	int getPackIndex(const SGString& _packName);
	int getPackIndexDefault(const char* _pPackName, int _defaultIndex = InvalidValue_v);
	int getPackIndexDefault(const SGString& _packName, int _defaultIndex = InvalidValue_v);

	int getImgIndexUnsafe(int _sgaIndex, const char* _pImgName);
	int getImgIndexUnsafe(int _sgaIndex, const SGString& _imgName);

	SGSize getTextureSize(int _packIndex, int _imgIndex, int _frameIndex);

	void releaseFrameTexture(int _packIndex, int _imgIndex, int _frameIndex);
	void releaseFrameTexture(const SgaResourceIndex& _sgaResourceIndex);
	void releaseAllFrameTexture();

	void increaseCounter();
	void decreaseCounter();
	void resetCounter();
	CachedTextureCounter& getCounter() { return textureCounter_; }

	void logTexture(const SGString& _text, const SgaResourceIndex& _resource, JCore::LoggerAbstract::Level _logLevel);

private:
	bool IsValidPackIndex(int _index);

	SGHashMap<SGString, int> pathToIdMap_;
	ImagePack* loadedPackages_[Const::Resource::MaxSgaFileCount]{};
	ImagePack* avatarPacks_[CharType::Max][AvatarType::Max]{};
	ImagePack* weaponPacks_[WeaponType::Max]{};
	CachedTextureCounter textureCounter_;
	int loadedPackageCount_{};
};
