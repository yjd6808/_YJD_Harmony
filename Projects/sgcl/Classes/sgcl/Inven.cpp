/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 10:32:04 PM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "Inven.h"

#include <bitset>

#include <sg/CharCommonInfo.h>

#include <sgcl/DataManager.h>
#include <sgcl/VisualHelper.h>

USING_NS_JC;
USING_NS_JS;
USING_NS_CC;

//////////////////////////////////////////////////////////////////////////////////////////
Inven::Inven()
: equippedItemList_{}
, equippedAvatarList_{}
, invenMap_{}
, availableSlotCount_{}
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Inven::~Inven()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void Inven::Init()
{
	// TODO: 우선 디폴트로 채워놓음
	CharCommonInfo* pCommon = sg::DataManager->GetCharCommonInfo(1);

	for (int i = 0; i < InvenItemType::Max; ++i)
	{
		availableSlotCount_[i] = pCommon->defaultInvenSlotCount_[i];
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
WeaponType_t Inven::GetWeaponType()
{
	InvenItemEquip* pItemWeapon = equippedItemList_[ItemType::Weapon];

	if (pItemWeapon == nullptr)
	{
		return sg::Contents.Player->GetBaseInfo()->defaultWeaponType_;
	}

	std::bitset<128> g;
	return pItemWeapon->code_.weaponUn_.weaponType_;
}

//////////////////////////////////////////////////////////////////////////////////////////
VisualInfo Inven::GetVisualInfo(int _defaultCharType)
{
	VisualInfo info;

	CharInfo* pCharInfo = sg::DataManager->GetCharInfo(_defaultCharType);
	bool equipped[VisualType::Max]{}; // 착용중인지
	VisualData data;

	// 아바타 정보 확인
	for (int i = VisualType::AvatarBegin; i <= VisualType::AvatarEnd; ++i)
	{
		if (equippedAvatarList_[i] == nullptr)
		{
			continue;
		}

		int visualCount = VisualHelper::GetVisualData(data, equippedAvatarList_[i]->code_.Code);
		info.PushBack(&data[0], visualCount);
		equipped[i] = true;
	}

	// 무기 정보 확인
	InvenItemEquip* pItemWeapon = equippedItemList_[ItemType::Weapon];

	if (pItemWeapon != nullptr)
	{
		int visualCount = VisualHelper::GetVisualData(data, pItemWeapon->code_.Code);
		info.PushBack(&data[0], visualCount);
		equipped[VisualType::Weapon] = true;
	}

	// 무기 강화 정보 확인

	for (int i = 0; i < VisualType::Max; ++i)
	{
		if (!pCharInfo->hasVisual_[i])
		{
			continue;
		}

		if (equipped[i])
		{
			continue;
		}

		// 디폴트는 존재하고 낀 비주얼 장비가 없으면 교체한다.
		info.PushBack(&pCharInfo->visual_[i][0], pCharInfo->visualCount_[i]);
	}

	return info;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Inven::GetAvailableSlotCount(InvenItemType_t _invenType)
{
	return availableSlotCount_[_invenType];
}
