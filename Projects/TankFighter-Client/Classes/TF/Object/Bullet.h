#pragma once

#include <TF/SourceHeader.h>

class Bullet : public Sprite
{
public:
	Bullet(int characterUID)					{ m_BulletInfo.CharacterUID = characterUID; }
	static Bullet* create(float diameter, int characterUID);
	bool init(float diameter);
	void update(float delta) override;
	void UpdateBulletInfo(BulletInfo& info);
	int GetCharacterUID() const					{ return m_BulletInfo.CharacterUID; }
	void LoadBulletInfo(Out_ BulletInfo& info);
	float GetDiameter() const					{ return m_BulletInfo.Diameter;}
private:
	BulletInfo m_BulletInfo{
		INVALID_UID,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		TANK_BULLET_SPEED,
		0
	};
};

