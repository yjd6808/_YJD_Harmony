/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:52:57 AM
 * =====================
 * 따로 설정파일에 정리했거나 하지 않은 변수들 모음
 */


#pragma once


#include "Tutturu.h"
#include <SteinsGate/Client/SGConfig.h>

class SGSpriteFrameTexture;
class SGGlobal
{
public:
	static SGGlobal* get() {
		static SGGlobal* loader;

		if (loader == nullptr) {
			loader = dbg_new SGGlobal;
			loader->initialize();
		}
		return loader;
	}

	SGGlobal();
	~SGGlobal();

	int convertAvatarPartNameToType(const SGString& str);
	void toggleDrawThicknessBox();
	void toggleDrawBodyBoundingBox();
	void toggleDrawAttackBox();
	void toggleDrawEffect();
	bool isThicknessBoxDrawMode() { return m_bDrawThicknessBox; }
	bool isBodyBoundingBoxDrawMode() { return m_bDrawBodyBoundingBox; }
	bool isAttackBoxDrawMode() { return m_bDrawAttackBox; }
	bool isEffectDrawMode() { return m_bDrawEffect; }

	const SGString& getAvatarSgaName(int charType, int avatarType);
	const SGString& getWeaponSgaName(int weaponType);

	SGSpriteFrameTexture* getDefaultFrameTexture();
private:
	void initialize();

	// 런타임 중 변경가능
	bool m_bDrawThicknessBox;
	bool m_bDrawBodyBoundingBox;
	bool m_bDrawAttackBox;
	bool m_bDrawEffect;

	// 변경되지 않는 데이터
	SGString m_AvatarPackName[CharType::Max][AvatarType::Max];	
	SGString m_WeaponPackName[WeaponType::Max];
	SGSpriteFrameTexture* m_pDefaultFrameTexture;
	
};