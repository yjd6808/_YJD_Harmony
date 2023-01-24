/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:46:52 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGImagePack.h>

class SGImagePackManager
{
public:
	static SGImagePackManager* getInstance() {
		static SGImagePackManager* loader;
		if (loader == nullptr) {
			loader = new SGImagePackManager;
			loader->loadAllPackages();
		}
		return loader;
	}

	SGImagePackManager();
	~SGImagePackManager();

	void loadAllPackages();

	SGImagePack* getPack(const SGString& packName);
	SGImagePack* getPack(const int idx);
	SGImagePack* getAvatarPack(CharacterType_t characterType, AvatarType_t avatarType);
	SGImagePack* getWeaponPack(CharacterType_t characterType, WeaponType_t weaponType);
	int getAvatarPackIndex(CharacterType_t characterType, AvatarType_t avatarType);
	int getWeaponPackIndex(CharacterType_t characterType, WeaponType_t weaponType);
	int getPackIndex(const SGString& packPath);

private:

	SGHashMap<SGString, int> m_PathToIdMap;
	
	SGImagePack* m_LoadedPackages[MaxNpkFileCount_v]{};
	SGImagePack* m_AvatarPacks[CharacterType::Max][AvatarType::Max]{};
	SGImagePack* m_WeaponPacks[CharacterType::Max][WeaponType::Max]{};
	int m_iLoadedPackageCount{};
};


