/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:59:34 PM
 * =====================
 *
 */

#include "Core.h"
#include "Enum_CHARACTER.h"

//////////////////////////////////////////////////////////////////////////////////////////
WeaponType_t WeaponType::getType(const jc::String& _name)
{
	static jc::HashMap<jc::String, WeaponType_t> nameToTypeMap
	{
		{ "auto", Automatic },
		{ "rev", Revolver },
		{ "bowgun", Bowgun },
		{ "musket", Musket },
		{ "hcan", HandCannon }
	};

	jc_assert_msg(nameToTypeMap.Exist(_name), "%s 무기타입명에 해당하는 코드가 존재하지 않습니다.", _name.Source());
	return nameToTypeMap[_name];
}
