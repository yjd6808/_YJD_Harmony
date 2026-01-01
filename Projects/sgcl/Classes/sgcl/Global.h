/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:52:57 AM
 * =====================
 * 따로 설정파일에 정리했거나 하지 않은 변수들 모음
 */


#pragma once


#include <sgcl/Core.h>

class SpriteFrameTexture;

class Global : public jc::SingletonPointer<Global>
{
private:
	friend class TSingleton;
	Global();
	~Global();

public:
	void init();
	int convertAvatarPartNameToType(const jc::String& _avatarPartName);
	void toggleDrawThicknessBox();
	void toggleDrawBodyBoundingBox();
	void toggleDrawAttackBox();
	void toggleDrawEffect();
	void toggleDrawUIStatic();

	const jc::String& getAvatarSgaName(int _charType, int _avatarType);
	const jc::String& getWeaponSgaName(int _weaponType);

	SpriteFrameTexture* getDefaultFrameTexture();

public:
	bool DrawThicknessBox;
	bool DrawBodyBoundingBox;
	bool DrawAttackBox;
	bool DrawEffect;
	bool DrawUIStatic;

private:
	// 런타임 중 변경가능

	// 변경되지 않는 데이터
	jc::String avatarPackName_[CharType::Max][AvatarType::Max];
	jc::String weaponPackName_[WeaponType::Max];
	SpriteFrameTexture* pDefaultFrameTexture_;
};
