/*
 * 작성자: 윤정도
 * 생성일: 1/2/2026 12:04:30 AM
 * =====================
 * cocos2d-x 기본 수학/타입 관련 코드들 분리함. 내가 만든 프로젝트들에도 모두 사용하도록 하기 위함.
 */

#pragma once

#include <algorithm>
#include <functional>
#include <cmath>
#include <memory>
#include <string>

#include <jc/Type.h>

#ifndef NS_CC_BEGIN
#define NS_CC_BEGIN  namespace cocos2d {
#endif

#ifndef NS_CC_END
#define NS_CC_END  }
#endif

#include <sgcc/math/CCMath.h>
#include <sgcc/math/CCGeometry.h>
#include <sgcc/base/ccTypesBase.h>
#include <sgcc/DLLExport.h>

namespace cocos2d
{
	using vec2 = cocos2d::Vec2;
	using vec3 = cocos2d::Vec3;
	using vec4 = cocos2d::Vec4;
	using size = cocos2d::Size;
	using rect = cocos2d::Rect;
}

namespace cc = cocos2d;