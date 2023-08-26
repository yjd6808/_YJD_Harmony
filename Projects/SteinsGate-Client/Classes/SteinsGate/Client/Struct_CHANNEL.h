/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 1:25:25 PM
 * =====================
 *
 */


#pragma once

struct ChannelDensityEx : ChannelDensity
{
	inline const static SGColor3B Color[Max]{
		SGColor3B{ 34, 139, 34 },	// 쾌적
		SGColor3B{ 237, 145, 33 },	// 보통
		SGColor3B{ 178, 34, 34 },	// 혼잡
		SGColor3B{ 255, 64, 169 }	// 포화
	};
};