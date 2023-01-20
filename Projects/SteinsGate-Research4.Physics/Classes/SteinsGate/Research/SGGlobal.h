/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:52:57 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/Config.h>

struct SGGlobal
{
public:
	static SGGlobal* getInstance() {
		static SGGlobal* loader;

		if (loader == nullptr) {
			loader = new SGGlobal;
			loader->initialize();
		}
		return loader;
	}

	int convertAvatarPartNameToType(const SGString& str);

	const SGString& getAvatarNpkName(int characterType, int visualAvatarType);
	const SGString& getWeaponNpkName(int characterType, int visualAvatarType);
private:
	void initialize();

	SGHashMap<SGString, int> AvatarPartNameToTypeMap;
	SGHashMap<int, SGString> AvatarPrefix[CharacterType::Max];
	SGString m_AvatarPackName[CharacterType::Max][AvatarType::VisualMax];	// 무기 제외
	SGString m_WeaponPackName[CharacterType::Max][WeaponType::Max];
};
