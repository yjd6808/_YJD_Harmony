/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:59:34 PM
 * =====================
 *
 */

#include "Core.h"
#include "Enum_CHARACTER.h"



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

