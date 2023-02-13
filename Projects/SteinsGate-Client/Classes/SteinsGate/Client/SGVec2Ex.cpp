/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:22:28 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGVec2Ex.h"

void SGVec2Ex::getLookDirection(const SGVec2& from, const SGVec2& to, Out_ Direction_t& lr, Out_ Direction_t& ud) {
	lr = Direction::Left;
	ud = Direction::Down;

	if (to.x - from.x > 0.0f) {
		lr = Direction::Right;
	}

	if (to.y - from.y > 0.0f) {
		ud = Direction::Up;
	}
}

void SGVec2Ex::getLookDirectionX(float from_x, float to_x, SpriteDirection_t& lr) {
	lr = SpriteDirection::Left;

	if (to_x - from_x > 0.0f) {
		lr = SpriteDirection::Right;
	}
}

void SGVec2Ex::getLookDirectionX(const SGVec2& from, const SGVec2& to, SpriteDirection_t& lr) {
	lr = SpriteDirection::Left;

	if (to.x - from.x > 0.0f) {
		lr = SpriteDirection::Right;
	}
}
