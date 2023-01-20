/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Enums.h>

struct SGCharacterBaseInfo;
struct SGCharacterBaseInfoLoader
{
public:
	static void LoadCharacterBaseInfo(SGCharacterBaseInfo(&characterInfoMap)[CharacterType::Max]);
};