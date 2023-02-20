/*
 * 작성자: 윤정도
 * 생성일: 2/19/2023 7:23:14 PM
 * =====================
 *
 */


#include "Core.h"
#include "SGEnums.h"




WeaponType_t WeaponType::getType(const JCore::String& name) {
	static SGHashMap<SGString, WeaponType_t> NameToTypeMap
	{
		{ "auto"	, Automatic		},
		{ "rev" 	, Revolver		},
		{ "bowgun"	, Bowgun		},
		{ "musket"	, Musket		},
		{ "hcan"	, HandCannon	}
	};

	DebugAssertMsg(NameToTypeMap.Exist(name), "%s 무기타입명에 해당하는 코드가 존재하지 않습니다.", name.Source());
	return NameToTypeMap[name];
}


//ItemOptType_t ItemOptType::Code(const char* codeName) {
//	static SGHashMap<const char*, ItemOptType_t> NameToCodeMap{
//		{ "str"									, Str							},
//		{ "dex"									, Dex							},
//		{ "int"									, Int							},
//		{ "vit"									, Vit							},
//		{ "lift"								, Lift							},
//		{ "mana"								, Mana							},
//		{ "damage_physical"						, DamagePhysical				},
//		{ "damage_magic"						, DamageMagic					},
//		{ "damage_physicl_reduce"				, DamagePhysiclReduce			},
//		{ "damage_physicl_reduce_percent"		, DamagePhysiclReducePercent	},
//		{ "damage_magic_reduce"					, DamageMagicReduce				},
//		{ "damage_magic_reduce_percent"			, DamageMagicReducePercent		},
//		{ "armor_physical"						, ArmorPhysical					},
//		{ "armor_magic"							, ArmorMagic					},
//		{ "chance_magic"						, ChanceMagic					},
//		{ "chance_gold"							, ChanceGold					},
//		{ "attack_speed"						, AttackSpeed					},
//		{ "cast_speed"							, CastSpeed						},
//		{ "regen_life_per_min"					, RegenLifePerMin				},
//		{ "regen_mana_per_min"					, RegenManaPerMin				},
//		{ "move_speed_in_dungeon"				, MoveSpeedInDungeon			},
//		{ "move_speed_in_town"					, MoveSpeedInTown				}
//	};
//
//	return NameToCodeMap[codeName];
//}
//
//ItemOptType_t ItemOptType::Code(const JCore::String& codeName) {
//	return Code(codeName.Source());
//}
