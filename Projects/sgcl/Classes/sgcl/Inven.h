/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 10:31:16 PM
 * =====================
 *
 */


#pragma once

#include <sgcl/Core.h>
#include <sg/_Struct/SteinsGate_Item.h>

class Inven final : public jc::SingletonPointer<Inven>
{
	friend class TSingleton;
	Inven();
	~Inven();

public:
	void Init();
	WeaponType_t	GetWeaponType();
	VisualInfo		GetVisualInfo(int _defaultCharType);

	int				GetAvailableSlotCount(InvenItemType_t _invenType);

private:
	InvenItemEquip* equippedItemList_[ItemType::MaxEquip];
	InvenItemEquip* equippedAvatarList_[AvatarType::Max];

	jc::HashMap<int, InvenItem*> invenMap_[InvenItemType::Max]; // 슬롯(키), 아이템

	int availableSlotCount_[InvenItemType::Max];
};
