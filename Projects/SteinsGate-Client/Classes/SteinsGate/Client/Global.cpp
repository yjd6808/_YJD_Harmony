/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:56:13 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "Global.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/FrameTexture.h>

USING_NS_JC;
USING_NS_CC;

Global::Global()
	: DrawThicknessBox(false)
	, DrawBodyBoundingBox(false)
	, DrawAttackBox(false)
	, DrawEffect(true)
	, DrawUIStatic(false)
	, m_pDefaultFrameTexture(nullptr)
{}

Global::~Global() {
	CC_SAFE_RELEASE(m_pDefaultFrameTexture);
}

int Global::convertAvatarPartNameToType(const SGString& str) {

	return -1;
}


void Global::toggleDrawThicknessBox() {
	DrawThicknessBox = !DrawThicknessBox;
}

void Global::toggleDrawBodyBoundingBox() {
	DrawBodyBoundingBox = !DrawBodyBoundingBox;
}

void Global::toggleDrawAttackBox() {
	DrawAttackBox = !DrawAttackBox;
}

void Global::toggleDrawEffect() {
	DrawEffect = !DrawEffect;
}

void Global::toggleDrawUIStatic() {
	DrawUIStatic = !DrawUIStatic;
}

void Global::init() {
	

	for (int i = 0; i < CharType::Max; ++i) {


		for (int j = 0; j < AvatarType::Max; ++j) {
			m_AvatarPackName[i][j] = StringUtil::Format("%s_avatar_%s.sga", CharType::Name[i], AvatarType::Name[j]);
		}

	}

	m_WeaponPackName[WeaponType::Automatic] = "gunner_weapon_auto.sga";
	m_WeaponPackName[WeaponType::Bowgun] = "gunner_weapon_bowgun.sga";
	m_WeaponPackName[WeaponType::HandCannon] = "gunner_weapon_hcan.sga";
	m_WeaponPackName[WeaponType::Musket] = "gunner_weapon_musket.sga";
	m_WeaponPackName[WeaponType::Revolver] = "gunner_weapon_rev.sga";


}



const SGString& Global::getAvatarSgaName(int charType, int avatarType) {
	DebugAssertMsg(charType >= CharType::Begin && charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다. [1]");
	DebugAssertMsg(avatarType >= AvatarType::Begin && avatarType < AvatarType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_AvatarPackName[charType][avatarType];
}

const SGString& Global::getWeaponSgaName(int weaponType) {
	DebugAssertMsg(weaponType >= WeaponType::Begin && weaponType < WeaponType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_WeaponPackName[weaponType];
}

SpriteFrameTexture* Global::getDefaultFrameTexture() {

	if (m_pDefaultFrameTexture != nullptr) {
		return m_pDefaultFrameTexture;
	}

	static unsigned char White2x2TextureData_v[] = {
		// RGBA8888
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF
	};

	
	SGTexture* pDefaultTexture = dbg_new SGTexture;
	pDefaultTexture->initWithData(White2x2TextureData_v, sizeof(White2x2TextureData_v), SGTexture::PixelFormat::RGB888, 2, 2, { 2, 2 });
	m_pDefaultFrameTexture = dbg_new SpriteFrameDefaultTexture(pDefaultTexture, { 0, 0, 2, 2, 2, 2 } );
	m_pDefaultFrameTexture->autorelease();
	m_pDefaultFrameTexture->retain();

	return m_pDefaultFrameTexture;
}
