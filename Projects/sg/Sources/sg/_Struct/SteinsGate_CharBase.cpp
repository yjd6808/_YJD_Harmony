/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#include "Core.h"
#include "SteinsGate_CharBase.h"

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

