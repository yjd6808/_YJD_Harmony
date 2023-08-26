/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 12:58:37 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Namepsace.h>

NS_CONST_BEGIN

namespace Map
{
	constexpr float TileWidth = 224;			// 맵 타일들 모두 가로길이가 224픽셀임
	constexpr float TileHeight = 184;			// 내가 자른 타일 높이
	constexpr int MapAreaBlockSize = 20;		// 16 고려해볼 것 타일 가로길이가 다 224임. 일단 테스트 끝나고 수정
}

NS_CONST_END