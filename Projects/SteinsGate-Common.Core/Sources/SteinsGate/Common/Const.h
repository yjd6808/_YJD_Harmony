/*
 * 작성자: 윤정도
 * 생성일: 2/13/2023 3:52:45 AM
 * =====================
 *
 */


#pragma once

#include <Steinsgate/Common/Namepsace.h>

constexpr int InvalidValue_v = -1;

NS_CONST_BEGIN

// 헤더파일
namespace FPS
{
	constexpr float _1 = 1.0f / 60.0f;
	constexpr float _2 = 2.0f / 60.0f;
	constexpr float _3 = 3.0f / 60.0f;
	constexpr float _4 = 4.0f / 60.0f;
	constexpr float _5 = 5.0f / 60.0f;
	constexpr float _6 = 6.0f / 60.0f;		// 0.1
	constexpr float _7 = 7.0f / 60.0f;
	constexpr float _8 = 8.0f / 60.0f;
	constexpr float _9 = 9.0f / 60.0f;
	constexpr float _10 = 10.0f / 60.0f;
	constexpr float _11 = 11.0f / 60.0f;
	constexpr float _12 = 12.0f / 60.0f;	// 0.2
	constexpr float _13 = 13.0f / 60.0f;
	constexpr float _14 = 14.0f / 60.0f;
	constexpr float _15 = 15.0f / 60.0f;
	constexpr float _16 = 16.0f / 60.0f;
	constexpr float _17 = 17.0f / 60.0f;
	constexpr float _18 = 18.0f / 60.0f;	// 0.3	
}

namespace Resource
{
	constexpr const char* ImageDirName = "image";
	constexpr const char* SoundDirName = "sound";
	constexpr const char* FontDirName = "font";

	constexpr int MaxSgaFileCount = 200;
	constexpr int MaxSgaParallelLoadingThreadCount = 8;
}


namespace Action
{
	inline constexpr int ComboSequenceCount = 5;					// 콤보 가능키 최대 몇개?
	inline constexpr float ComboSequenceDelay = FPS::_18;			// 콤보 입력 제한 시간
}

namespace Character
{
}

namespace StringLen
{
	constexpr int CharacterName = 16;
	constexpr int AccountId = 16;
	constexpr int AccountPass = 16;
}

namespace Map
{
	constexpr float TileWidth = 224;			// 맵 타일들 모두 가로길이가 224픽셀임
	constexpr float TileHeight = 184;			// 내가 자른 타일 높이
	constexpr int MapAreaBlockSize = 20;		// 16 고려해볼 것 타일 가로길이가 다 224임. 일단 테스트 끝나고 수정
}
	
namespace Item
{
	constexpr int MaxOptCount = 7;
	constexpr int MaxEnchantLevel = 25;
	constexpr int ArmorEnchantBonusOptCount = 2;
}

namespace Visual
{
	constexpr int MaxShapeCount = 3;			// 
}


namespace Server
{
	inline constexpr int MaxId = 32;
}

namespace Monster
{
	constexpr int MaxMonsterPartsCount_v = 5;				// 일단 5개로 둠.
	constexpr int MaxMapObjectGatePartsCount_v = 3;			// 문 테두리, 문, 빛 게이트는 이렇게 3개의 파츠로 구성됨

}

NS_CONST_END