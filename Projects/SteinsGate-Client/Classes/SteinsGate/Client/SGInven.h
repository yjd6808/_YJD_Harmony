/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 10:31:16 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

class SGInven
{
public:
	SGInven();

	static SGInven* get();

	WeaponType_t getWeaponType();
	void getVisualInfo(Out_ VisualInfo& info, int defaultCharType);

private:
	InvenItemEquip* m_EquipedItemList[ItemType::MaxEquip];
	InvenItemEquip* m_EquipedAvatarList[AvatarType::Max];

	SGHashMap<int, InvenItem*> m_hashInven[InvenItemType::Max];		// 슬롯(키), 아이템
};


