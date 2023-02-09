#pragma once

#include <TF/SourceHeader.h>
class CircleSprite : public Sprite
{
public:
	CircleSprite(const Vec2& origin, const float radious) : m_Origin(origin), m_fRadious(radious) {}

	static CircleSprite* create(const Vec2& origin, const float radious);
	bool init(const Vec2& origin, const float radious);
private:
	Vec2 m_Origin{};
	float m_fRadious{};
};

inline CircleSprite* CircleSprite::create(const std::string& spriteName, const Vec2& origin, const float radious) {
	CircleSprite* sprite = new CircleSprite(origin, radious);
	if (sprite && sprite->initWithFile(spriteName))
	{
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

inline bool CircleSprite::init(const Vec2& origin, const float radious) {

}
