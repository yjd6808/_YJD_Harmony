/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:52:57 AM
 * =====================
 * 따로 설정파일에 정리했거나 하지 않은 변수들 모음
 */


#pragma once


#include <SteinsGate/Client/Tutturu.h>

class SpriteFrameTexture;

class Global : public JCore::SingletonPointer<Global>
{
private:
	friend class TSingleton;
	Global();
	~Global();

public:
	void init();
	int convertAvatarPartNameToType(const SGString& _avatarPartName);
	void toggleDrawThicknessBox();
	void toggleDrawBodyBoundingBox();
	void toggleDrawAttackBox();
	void toggleDrawEffect();
	void toggleDrawUIStatic();

	const SGString& getAvatarSgaName(int _charType, int _avatarType);
	const SGString& getWeaponSgaName(int _weaponType);

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
	SGString avatarPackName_[CharType::Max][AvatarType::Max];
	SGString weaponPackName_[WeaponType::Max];
	SpriteFrameTexture* pDefaultFrameTexture_;
};
