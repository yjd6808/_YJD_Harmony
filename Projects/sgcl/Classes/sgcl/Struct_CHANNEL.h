/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 1:25:25 PM
 * =====================
 *
 */


#pragma once

struct ChannelDensityEx : ChannelDensity
{
	inline const static cc::Color3B Color[Max]{
		cc::Color3B{ 34, 139, 34 }, // 쾌적
		cc::Color3B{ 237, 145, 33 }, // 보통
		cc::Color3B{ 178, 34, 34 }, // 혼잡
		cc::Color3B{ 255, 64, 169 } // 포화
	};
};
