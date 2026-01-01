/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */

#pragma once

#include <sg/Struct_ACTOR.h>
#include <sg/Struct_ANIMATION.h>
#include <sg/Struct_AUTHENTICATION.h>
#include <sg/Struct_CHANNEL.h>
#include <sg/Struct_ITEM.h>
#include <sg/Struct_RESOURCE.h>

struct PlayerData
{
	PlayerData();

	void clear();

	int CharId; // 캐릭터 고유 아이디
	jc::StaticString<Const::StringLen::CharacterName> Name;
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
