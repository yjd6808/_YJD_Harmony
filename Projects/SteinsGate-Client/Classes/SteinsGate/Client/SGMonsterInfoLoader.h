/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:11 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGMonsterInfo.h>


struct SGMonsterInfoLoader
{
public:
	static bool LoadMonsterInfo(SGHashMap<int, SGMonsterInfo>& monsterInfoMap);
};
