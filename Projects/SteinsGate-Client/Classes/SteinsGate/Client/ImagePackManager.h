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
	void unloadPackData(int packIndex);

	ImagePack* getPack(const SGString& packName);
	ImagePack* getPack(const char* packName);
	ImagePack* getPackUnsafe(const SGString& packName);
	ImagePack* getPackUnsafe(const char* packName);
	ImagePack* getPack(const int idx);
	ImagePack* getPackUnsafe(const int idx);
	ImagePack* getAvatarPack(CharType_t charType, AvatarType_t avatarType);
	ImagePack* getWeaponPack(WeaponType_t weaponType);

	int getAvatarPackIndex(CharType_t charType, AvatarType_t avatarType);
	int getWeaponPackIndex(WeaponType_t weaponType);
	int getPackIndex(const SGString& packName);
	int getPackIndexDefault(const char* packName, int defaultIndex = InvalidValue_v);
	int getPackIndexDefault(const SGString& packName, int defaultIndex = InvalidValue_v);

	int getImgIndexUnsafe(int sgaIndex, const char* imgName);
	int getImgIndexUnsafe(int sgaIndex, const SGString& imgName);

	SGSize getTextureSize(int packIndex, int imgIndex, int frameIndex);

	void releaseFrameTexture(int packIndex, int imgIndex, int frameIndex);
	void releaseFrameTexture(const SgaResourceIndex& sgaResourceIndex);
	void releaseAllFrameTexture();

	void increaseCounter();
	void decreaseCounter();
	void resetCounter();
	CachedTextureCounter& getCounter() { return m_TextureCounter; }

	void logTexture(const SGString& text, const SgaResourceIndex& resource, JCore::LoggerAbstract::Level logLevel);

	// TODO: 로딩된 패키지들 모두 Unload 하는 기능
	// void unloadAll();
private:
	bool IsValidPackIndex(int index);

	SGHashMap<SGString, int> m_PathToIdMap;
	
	ImagePack* m_LoadedPackages[Const::Resource::MaxSgaFileCount]{};
	ImagePack* m_AvatarPacks[CharType::Max][AvatarType::Max]{};
	ImagePack* m_WeaponPacks[WeaponType::Max]{};
	CachedTextureCounter m_TextureCounter;
	int m_iLoadedPackageCount{};
};


