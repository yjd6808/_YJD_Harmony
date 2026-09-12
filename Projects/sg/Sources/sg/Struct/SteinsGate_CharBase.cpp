/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#include "SteinsGate_CharBase.h"

//////////////////////////////////////////////////////////////////////////////////////////
WeaponType_t WeaponType::getType(const jc::String& _name)
{
	static jc::HashMap<jc::String, WeaponType_t> nameToTypeMap
	{
		{ _T("auto"), Automatic },
		{ _T("rev"), Revolver },
		{ _T("bowgun"), Bowgun },
		{ _T("musket"), Musket },
		{ _T("hcan"), HandCannon }
	};

	jc_assert_msg(nameToTypeMap.Exist(_name), _T("%s 무기타입명에 해당하는 코드가 존재하지 않습니다."), _name.Source());
	return nameToTypeMap[_name];
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActorRect::IsCollide(const ActorRect& _rect, SpriteDirection_t& _otherHitDirection, cc::rect& _hitRect)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSpriteData::ActorPartSpriteData()
: sgaIndex_(InvalidValue_v)
, imgIndex_(InvalidValue_v)
, zOrder_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSpriteData::ActorPartSpriteData(int _zOrder, int _sga, int _img)
: sgaIndex_(_sga)
, imgIndex_(_img)
, zOrder_(_zOrder)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
AccountData::AccountData()
: id_{ _T("") }
, pass_{ _T("") }
, lastLoginTime_(0)
, lastServer_(GameServerType::Max)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AccountData::Clear()
{
	id_ = _T("");
	pass_ = _T("");
	lastLoginTime_.Tick = 0;
	lastServer_ = GameServerType::Max;
}
