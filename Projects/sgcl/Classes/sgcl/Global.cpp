/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:56:13 AM
 * =====================
 *
 */


#include "Core.h"
#include "Global.h"
#include "GameCoreHeader.h"

#include <sgcl/FrameTexture.h>

USING_NS_JC;
USING_NS_CC;

//////////////////////////////////////////////////////////////////////////////////////////
Global::Global()
: DrawThicknessBox(false)
, DrawBodyBoundingBox(false)
, DrawAttackBox(false)
, DrawEffect(true)
, DrawUIStatic(false)
, pDefaultFrameTexture_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Global::~Global()
{
	CC_SAFE_RELEASE(pDefaultFrameTexture_);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Global::convertAvatarPartNameToType(const jc::String& _avatarPartName)
{
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Global::toggleDrawThicknessBox()
{
	DrawThicknessBox = !DrawThicknessBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Global::toggleDrawBodyBoundingBox()
{
	DrawBodyBoundingBox = !DrawBodyBoundingBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Global::toggleDrawAttackBox()
{
	DrawAttackBox = !DrawAttackBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Global::toggleDrawEffect()
{
	DrawEffect = !DrawEffect;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Global::toggleDrawUIStatic()
{
	DrawUIStatic = !DrawUIStatic;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Global::init()
{
	for (int i = 0; i < CharType::Max; ++i)
	{
		for (int j = 0; j < AvatarType::Max; ++j)
		{
			avatarPackName_[i][j] = StringUtil::Format("%s_avatar_%s.sga", CharType::Name[i], AvatarType::Name[j]);
		}
	}

	weaponPackName_[WeaponType::Automatic] = "gunner_weapon_auto.sga";
	weaponPackName_[WeaponType::Bowgun] = "gunner_weapon_bowgun.sga";
	weaponPackName_[WeaponType::HandCannon] = "gunner_weapon_hcan.sga";
	weaponPackName_[WeaponType::Musket] = "gunner_weapon_musket.sga";
	weaponPackName_[WeaponType::Revolver] = "gunner_weapon_rev.sga";
}

//////////////////////////////////////////////////////////////////////////////////////////
const jc::String& Global::getAvatarSgaName(int _charType, int _avatarType)
{
	jc_assert_msg(_charType >= CharType::Begin && _charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다. [1]");
	jc_assert_msg(_avatarType >= AvatarType::Begin && _avatarType < AvatarType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return avatarPackName_[_charType][_avatarType];
}

//////////////////////////////////////////////////////////////////////////////////////////
const jc::String& Global::getWeaponSgaName(int _weaponType)
{
	jc_assert_msg(_weaponType >= WeaponType::Begin && _weaponType < WeaponType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return weaponPackName_[_weaponType];
}

//////////////////////////////////////////////////////////////////////////////////////////
SpriteFrameTexture* Global::getDefaultFrameTexture()
{
	if (pDefaultFrameTexture_ != nullptr)
	{
		return pDefaultFrameTexture_;
	}

	static unsigned char white2x2TextureData[] = {
		// RGBA8888
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF
	};

	cc::Texture* pDefaultTexture = dbg_new cc::Texture;
	pDefaultTexture->initWithData(white2x2TextureData, sizeof(white2x2TextureData), cc::Texture::PixelFormat::RGB888, 2,
	                              2, { 2, 2 });
	pDefaultFrameTexture_ = dbg_new SpriteFrameDefaultTexture(pDefaultTexture, { 0, 0, 2, 2, 2, 2 });
	pDefaultFrameTexture_->autorelease();
	pDefaultFrameTexture_->retain();

	return pDefaultFrameTexture_;
}
