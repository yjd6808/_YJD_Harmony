/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 1:25:25 PM
 * =====================
 *
 */


#pragma once

struct ChannelDensityEx : ChannelDensity
{
	inline const static c2d::Color3B Color[Max]{
		c2d::Color3B{ 34, 139, 34 }, // 쾌적
		c2d::Color3B{ 237, 145, 33 }, // 보통
		c2d::Color3B{ 178, 34, 34 }, // 혼잡
		c2d::Color3B{ 255, 64, 169 } // 포화
	};
};
