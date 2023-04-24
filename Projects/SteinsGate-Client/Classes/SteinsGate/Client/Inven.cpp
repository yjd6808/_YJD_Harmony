/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 10:32:04 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "Inven.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/VisualHelper.h>

USING_NS_JC;
USING_NS_JS;
USING_NS_CC;

Inven::Inven()
	: m_EquipedItemList{}
	, m_EquipedAvatarList{}
	, m_hashInven{}
	, m_iAvailableSlotCount{} {
}

Inven* Inven::get() {
	static Inven* s_pInst;

	if (s_pInst == nullptr) {
		s_pInst = dbg_new Inven;
	}

	return s_pInst;
}

void Inven::init() {
	for (int i = 0; i < InvenItemType::Max; ++i) {
		m_iAvailableSlotCount[i] = CoreCharCommon_v->DefaultInvenSlotCount[i];
	}
}

WeaponType_t Inven::getWeaponType() {
	InvenItemEquip* pItemWeapon = m_EquipedItemList[ItemType::Weapon];

	if (pItemWeapon == nullptr) {
		return CorePlayer_v->getBaseInfo()->DefaultWeaponType;
	}

	return pItemWeapon->Code.WeaponUn.WeaponType;
}

void Inven::getVisualInfo(Out_ VisualInfo& info, int defaultCharType) {

	CharInfo* pCharInfo = CoreDataManager_v->getCharInfo(defaultCharType);
	bool bEquiped[VisualType::Max]{};	// 착용중인지
	VisualData data;

	// 아바타 정보 확인
	for (int i = VisualType::AvatarBegin; i <= VisualType::AvatarEnd; ++i) {
		if (m_EquipedAvatarList[i] == nullptr) {
			continue;
		}

		int iVisualCount = VisualHelper::getVisualData(data, m_EquipedAvatarList[i]->Code.Code);
		info.PushBack(&data[0], iVisualCount);
		bEquiped[i] = true;
	}

	// 무기 정보 확인
	InvenItemEquip* pItemWeapon = m_EquipedItemList[ItemType::Weapon];

	if (pItemWeapon != nullptr) {
		int iVisualCount = VisualHelper::getVisualData(data, pItemWeapon->Code.Code);
		info.PushBack(&data[0], iVisualCount);
		bEquiped[VisualType::Weapon] = true;
	}

	// 무기 강화 정보 확인
	
	for (int i = 0; i < VisualType::Max; ++i) {

		if (!pCharInfo->HasVisual[i])
			continue;

		if (bEquiped[i])
			continue;

		// 디폴트는 존재하고 낀 비주얼 장비가 없으면 교체한다.
		info.PushBack(&pCharInfo->Visual[i][0], pCharInfo->VisualCount[i]);
	}
}

int Inven::getAvailableSlotCount(InvenItemType_t invenType) {
	return m_iAvailableSlotCount[invenType];
}
