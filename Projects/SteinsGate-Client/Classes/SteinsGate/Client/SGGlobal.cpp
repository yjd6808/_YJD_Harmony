/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:56:13 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGGlobal.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/SGFrameTexture.h>

USING_NS_JC;
USING_NS_CC;

SGGlobal::SGGlobal()
	: m_pDefaultFrameTexture(nullptr)
	, m_bDrawThicknessBox(false)
	, m_bDrawBodyBoundingBox(false)
	, m_bDrawAttackBox(false)
	, m_bDrawEffect(true)
{}

SGGlobal::~SGGlobal() {
	CC_SAFE_RELEASE(m_pDefaultFrameTexture);
}


void SGGlobal::toggleDrawThicknessBox() {
	m_bDrawThicknessBox = !m_bDrawThicknessBox;
}

void SGGlobal::toggleDrawBodyBoundingBox() {
	m_bDrawBodyBoundingBox = !m_bDrawBodyBoundingBox;
}

void SGGlobal::toggleDrawAttackBox() {
	m_bDrawAttackBox = !m_bDrawAttackBox;
}

void SGGlobal::toggleDrawEffect() {
	m_bDrawEffect = !m_bDrawEffect;
}

void SGGlobal::initialize() {
	

	for (int i = 0; i < CharType::Max; ++i) {


		for (int j = 0; j < AvatarType::Max; ++j) {
			m_AvatarPackName[i][j] = StringUtil::Format("%s_avatar_%s.NPK", CharType::Name[i], AvatarType::Name[j]);
		}

	}

	m_WeaponPackName[WeaponType::Automatic] = "gunner_weapon_auto.NPK";
	m_WeaponPackName[WeaponType::Bowgun] = "gunner_weapon_bowgun.NPK";
	m_WeaponPackName[WeaponType::HandCannon] = "gunner_weapon_hcan.NPK";
	m_WeaponPackName[WeaponType::Musket] = "gunner_weapon_musket.NPK";
	m_WeaponPackName[WeaponType::Revolver] = "gunner_weapon_rev.NPK";


}



const SGString& SGGlobal::getAvatarNpkName(int charType, int avatarType) {
	DebugAssertMsg(charType >= CharType::Begin && charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다. [1]");
	DebugAssertMsg(avatarType >= AvatarType::Begin && avatarType < AvatarType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_AvatarPackName[charType][avatarType];
}

const SGString& SGGlobal::getWeaponNpkName(int weaponType) {
	DebugAssertMsg(weaponType >= WeaponType::Begin && weaponType < WeaponType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_WeaponPackName[weaponType];
}

SGSpriteFrameTexture* SGGlobal::getDefaultFrameTexture() {

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
	m_pDefaultFrameTexture = dbg_new SGSpriteFrameTexture(pDefaultTexture, { 0, 0, 2, 2, 2, 2 }, 0, true);
	m_pDefaultFrameTexture->autorelease();
	m_pDefaultFrameTexture->retain();

	return m_pDefaultFrameTexture;
}
