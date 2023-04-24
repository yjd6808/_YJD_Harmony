/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 10:31:16 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

class Inven
{
public:
	Inven();

	static Inven* get();

	void init();

	WeaponType_t getWeaponType();
	void getVisualInfo(Out_ VisualInfo& info, int defaultCharType);

	int getAvailableSlotCount(InvenItemType_t invenType);
private:
	InvenItemEquip* m_EquipedItemList[ItemType::MaxEquip];
	InvenItemEquip* m_EquipedAvatarList[AvatarType::Max];

	SGHashMap<int, InvenItem*> m_hashInven[InvenItemType::Max];		// 슬롯(키), 아이템

	int m_iAvailableSlotCount[InvenItemType::Max];
};


