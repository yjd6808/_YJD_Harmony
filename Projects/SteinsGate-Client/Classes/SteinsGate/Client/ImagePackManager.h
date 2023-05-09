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
	~ImagePackManager() override;
public:
	void loadAllPackages();
	void unloadPackData(int packIndex);

	ImagePack* getPack(const SGString& packName);
	ImagePack* getPackUnsafe(const SGString& packName);
	ImagePack* getPack(const int idx);
	ImagePack* getPackUnsafe(const int idx);
	ImagePack* getAvatarPack(CharType_t charType, AvatarType_t avatarType);
	ImagePack* getWeaponPack(WeaponType_t weaponType);

	int getAvatarPackIndex(CharType_t charType, AvatarType_t avatarType);
	int getWeaponPackIndex(WeaponType_t weaponType);
	int getPackIndex(const SGString& packPath);
	int getPackIndexDefault(const SGString& packPath, int defaultIndex);

	void releaseFrameTexture(int packIndex, int imgIndex, int frameIndex);
	void releaseFrameTexture(const SgaResourceIndex& sgaResourceIndex);

	// TODO: 로딩된 패키지들 모두 Unload 하는 기능
	// void unloadAll();
private:
	bool IsValidPackIndex(int index);

	SGHashMap<SGString, int> m_PathToIdMap;
	
	ImagePack* m_LoadedPackages[MaxSgaFileCount_v]{};
	ImagePack* m_AvatarPacks[CharType::Max][AvatarType::Max]{};
	ImagePack* m_WeaponPacks[WeaponType::Max]{};
	int m_iLoadedPackageCount{};
};


