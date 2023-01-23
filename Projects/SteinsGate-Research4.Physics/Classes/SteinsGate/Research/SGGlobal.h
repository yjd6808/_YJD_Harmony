/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:52:57 AM
 * =====================
 * 따로 설정파일에 정리했거나 하지 않은 변수들 모음
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

	SGGlobal();
	~SGGlobal();

	int convertAvatarPartNameToType(const SGString& str);
	void toggleDrawThicknessBox();
	void toggleDrawBodyBoundingBox();
	bool isThicknessBoxDrawMode() { return m_bDrawThicknessBox; }
	bool isBodyBoundingBoxDrawMode() { return m_bDrawBodyBoundingBox; }


	const SGString& getAvatarNpkName(int characterType, int visualAvatarType);
	const SGString& getWeaponNpkName(int characterType, int weaponType);

	SGTexture* getDefaultTexture();
private:
	void initialize();

	// 런타임 중 변경가능
	bool m_bDrawThicknessBox;
	bool m_bDrawBodyBoundingBox;

	// 변경되지 않는 데이터
	SGHashMap<SGString, int> m_AvatarPartNameToTypeMap;
	SGHashMap<int, SGString> m_AvatarPrefix[CharacterType::Max];
	SGString m_AvatarPackName[CharacterType::Max][AvatarType::Max];	
	SGString m_WeaponPackName[CharacterType::Max][WeaponType::Max];
	SGTexture* m_pDefaultTexture;
	
};
