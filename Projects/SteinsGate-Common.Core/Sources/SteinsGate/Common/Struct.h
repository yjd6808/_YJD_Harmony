/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Struct_ACTOR.h>
#include <SteinsGate/Common/Struct_ANIMATION.h>
#include <SteinsGate/Common/Struct_AUTHENTICATION.h>
#include <SteinsGate/Common/Struct_CHANNEL.h>
#include <SteinsGate/Common/Struct_ITEM.h>
#include <SteinsGate/Common/Struct_RESOURCE.h>




struct PlayerData
{
	PlayerData();

	void clear();

	int CharId;	// 캐릭터 고유 아이디
	SGStaticString<Const::StringLen::CharacterName> Name;
	CharType_t CharType;
	Int32 Life;
	Int32 MaxLife;
	Int32 Mana;
	Int32 MaxMana;
	Int32 Level;

	// 호스트 플레이어 데이터
	Int64 Gold;
	Int32 Str;
	Int32 Dex;
	Int32 Int;
	Int32 Vit;
};


