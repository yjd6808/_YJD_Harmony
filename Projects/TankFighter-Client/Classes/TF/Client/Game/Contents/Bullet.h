/*
 * 작성자: 윤정도
 * 생성일: 10/20/2023 5:52:31 PM
 * =====================
 *
 */


#pragma once

#include <cocos/ui/CocosGUI.h>

class Bullet : public cocos2d::ui::Scale9Sprite
{
public:
	Bullet();

	static Bullet* create();

	bool init() override;
	bool isOutOfMap() const;

	void update(float delta) override;

	void setCharacterPrimaryKey(int primaryKey);
	void setDiameter(float diameter);

	float getDiameter() const { return m_Info.Diameter; }
	BulletInfoNet getNetInfo() const;
	int getCharacterPrimaryKey() const { return m_Info.CharacterPrimaryKey; }

	void setNetInfo(const BulletInfoNet& bulletInfo);
private:
	BulletInfoNet m_Info;
};


