/*
 * 작성자: 윤정도
 * 생성일: 1/30/2023 7:57:22 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "AnimationInfo.h"

USING_NS_JC;

AnimationInfo::AnimationInfo(const AnimationInfo&) {
	DebugAssertMsg(false, "복사 생성 금지");	// 이 객체는 조회 전용 데이터이기땜
}

AnimationInfo::AnimationInfo(AnimationInfo&& other) noexcept {
	this->operator=(Move(other));
}

AnimationInfo& AnimationInfo::operator=(const AnimationInfo&) {
	DebugAssertMsg(false, "복사 대입 금지");
	return *this;
}

AnimationInfo& AnimationInfo::operator=(AnimationInfo&& other) noexcept {
	Code = other.Code;
	Loop = other.Loop;
	Name = Move(other.Name);
	Frames = Move(other.Frames);
	return *this;
}

AnimationInfo::~AnimationInfo() {
	for (int i = 0; i < Frames.Size(); ++i) {
		JCORE_DELETE_SAFE(Frames[i]);
	}
}

