/*
 * 작성자: 윤정도
 * 생성일: 1/30/2023 7:57:22 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGAnimationInfo.h"

USING_NS_JC;

SGAnimationInfo::SGAnimationInfo(const SGAnimationInfo&) {
	DebugAssertMessage(false, "복사 생성 금지");
}

SGAnimationInfo::SGAnimationInfo(SGAnimationInfo&& other) noexcept {
	this->operator=(Move(other));
}

SGAnimationInfo& SGAnimationInfo::operator=(const SGAnimationInfo&) {
	DebugAssertMessage(false, "복사 대입 금지");
	return *this;
}

SGAnimationInfo& SGAnimationInfo::operator=(SGAnimationInfo&& other) noexcept {
	Code = other.Code;
	Loop = other.Loop;
	Name = Move(other.Name);
	Frames = Move(other.Frames);
	return *this;
}

SGAnimationInfo::~SGAnimationInfo() {
	for (int i = 0; i < Frames.Size(); ++i) {
		DeleteSafe(Frames[i]);
	}
	
}

