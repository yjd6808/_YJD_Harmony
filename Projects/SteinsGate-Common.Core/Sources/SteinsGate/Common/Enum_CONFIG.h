/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 8:18:52 PM
 * =====================
 *
 */


#pragma once


#include <JCore/Define.h>

JCORE_SENUM_BEGIN(ConfigFileType)
Effect,
Begin = Effect,
Map,
MapPhysics,
MapObject,
Monster,
Monster_Animation_Frame_Event,
Monster_Attack_Data,
Monster_Projectile,
Monster_Stat,
Server,
Tile,
UI,
Action,
AI,
AttackBox,
Channel,
Char_Animation,
Char_Animation_Frame_Event,
Char_Attack_Data,
Char_Base,
Char_Common,
Char_Projectile,
Client,
ClientText,
Item,
ItemOpt,
Enchant,
Common,
Database,
End,
Max = End
JCORE_SENUM_MIDDLE(ConfigFileType)

static constexpr const char* FileName[Max]{
	"effect.json"						,		// 파일
	"map"								,		// 디렉토리
	"map_physics.json"					,		// 파일
	"map_object.json"					,		// 파일
	"monster.json"						,		// 파일
	"monster_animation_frame_event.json",		// 파일
	"monster_attack_data.json"			,		// 파일
	"monster_projectile.json"			,		// 파일
	"monster_stat.json"					,		// 파일
	"server.json"						,		// 파일
	"tile.json"							,		// 파일
	"ui.json"							,		// 파일
	"action.json"						,		// 파일
	"ai.json"							,		// 파일
	"attack_box.json"					,		// 파일
	"channel.json"						,		// 파일
	"char_animation.json"				,		// 파일
	"char_animation_frame_event.json"	,		// 파일
	"char_attack_data.json"				,		// 파일
	"char_base.json"					,		// 파일
	"char_common.json"					,		// 파일
	"char_projectile.json"				,		// 파일
	"client.json"						,		// 파일
	"client_text.json"					,		// 파일
	"item"								,		// 디렉토리
	"item_opt.json"						,		// 파일
	"enchant.json"						,		// 파일
	"common.json"						,		// 파일 common.json에서 다른 이름으로 변경시, 클라이언트와 각 서버프로젝트들 PostBuild이벤트로 수행하는 설정파일이름도 변경필요
	"database.json"								// 파일

};
JCORE_SENUM_MIDDLE_END(ConfigFileType)

