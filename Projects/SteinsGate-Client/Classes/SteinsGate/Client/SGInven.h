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

	void getVisualInfo(Out_ VisualInfo& info, int defaultCharType);

private:
	InvenItemEquip* m_EquipedItemList[EquipItemType::Max];
	InvenItemEquip* m_EquipedAvatarList[AvatarType::Max];

	SGHashMap<int, InvenItem*> m_hashInven[ItemType::Max];		// 슬롯(키), 아이템
};


