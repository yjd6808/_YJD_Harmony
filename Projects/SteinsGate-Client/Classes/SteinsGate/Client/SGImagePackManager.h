/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:46:52 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGImagePack.h>

class SGImagePackManager
{
public:
	static SGImagePackManager* get() {
		static SGImagePackManager* loader;
		if (loader == nullptr) {
			loader = dbg_new SGImagePackManager;
			loader->loadAllPackages();
		}
		return loader;
	}

	SGImagePackManager();
	~SGImagePackManager();

	void loadAllPackages();

	SGImagePack* getPack(const SGString& packName);
	SGImagePack* getPack(const int idx);
	SGImagePack* getAvatarPack(CharType_t charType, AvatarType_t avatarType);
	SGImagePack* getWeaponPack(CharType_t charType, WeaponType_t weaponType);
	int getAvatarPackIndex(CharType_t charType, AvatarType_t avatarType);
	int getWeaponPackIndex(CharType_t charType, WeaponType_t weaponType);
	int getPackIndex(const SGString& packPath);
	int getPackIndexDefault(const SGString& packPath, int defaultIndex);

	// TODO: 로딩된 패키지들 모두 Unload 하는 기능
	// void unloadAll();
private:

	SGHashMap<SGString, int> m_PathToIdMap;
	
	SGImagePack* m_LoadedPackages[MaxNpkFileCount_v]{};
	SGImagePack* m_AvatarPacks[CharType::Max][AvatarType::Max]{};
	SGImagePack* m_WeaponPacks[CharType::Max][5]{};
	int m_iLoadedPackageCount{};
};


