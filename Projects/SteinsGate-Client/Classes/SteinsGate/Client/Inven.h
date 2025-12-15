/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 10:31:16 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

class Inven final : public JCore::SingletonPointer<Inven>
{
	friend class TSingleton;
	Inven();
	~Inven();

public:
	void init();
	WeaponType_t getWeaponType();
	VisualInfo getVisualInfo(int _defaultCharType);

	int getAvailableSlotCount(InvenItemType_t _invenType);

private:
	InvenItemEquip* equippedItemList_[ItemType::MaxEquip];
	InvenItemEquip* equippedAvatarList_[AvatarType::Max];

	SGHashMap<int, InvenItem*> invenMap_[InvenItemType::Max]; // 슬롯(키), 아이템

	int availableSlotCount_[InvenItemType::Max];
};
