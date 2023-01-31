/*
 * 작성자: 윤정도
 * 생성일: 1/30/2023 7:57:22 PM
 * =====================
 *
 */


#include "SGAnimationInfo.h"


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

