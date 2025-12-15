/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 10:32:04 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "Inven.h"

#include <bitset>

#include "GameCoreHeader.h"

#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/VisualHelper.h>

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
void Inven::init()
{
	for (int i = 0; i < InvenItemType::Max; ++i)
	{
		availableSlotCount_[i] = Core::CharCommon->defaultInvenSlotCount_[i];
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
WeaponType_t Inven::getWeaponType()
{
	InvenItemEquip* pItemWeapon = equippedItemList_[ItemType::Weapon];

	if (pItemWeapon == nullptr)
	{
		return Core::Contents.Player->getBaseInfo()->defaultWeaponType_;
	}

	std::bitset<128> g;
	return pItemWeapon->Code.WeaponUn.WeaponType;
}

//////////////////////////////////////////////////////////////////////////////////////////
VisualInfo Inven::getVisualInfo(int _defaultCharType)
{
	VisualInfo info;

	CharInfo* pCharInfo = Core::DataManager->getCharInfo(_defaultCharType);
	bool equipped[VisualType::Max]{}; // 착용중인지
	VisualData data;

	// 아바타 정보 확인
	for (int i = VisualType::AvatarBegin; i <= VisualType::AvatarEnd; ++i)
	{
		if (equippedAvatarList_[i] == nullptr)
		{
			continue;
		}

		int visualCount = VisualHelper::getVisualData(data, equippedAvatarList_[i]->Code.Code);
		info.PushBack(&data[0], visualCount);
		equipped[i] = true;
	}

	// 무기 정보 확인
	InvenItemEquip* pItemWeapon = equippedItemList_[ItemType::Weapon];

	if (pItemWeapon != nullptr)
	{
		int visualCount = VisualHelper::getVisualData(data, pItemWeapon->Code.Code);
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
int Inven::getAvailableSlotCount(InvenItemType_t _invenType)
{
	return availableSlotCount_[_invenType];
}
