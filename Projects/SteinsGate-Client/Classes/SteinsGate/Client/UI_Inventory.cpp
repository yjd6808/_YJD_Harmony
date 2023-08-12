/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:34 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UI_Inventory.h"

#include <SteinsGate/Client/Define_UI.h>

USING_NS_CC;
USING_NS_JC;


UI_Inventory::UI_Inventory(UIGroupInfo* groupInfo)
	: UIMasterGroup(groupInfo)
	, m_pBackground(nullptr)
	, m_pScrollBar(nullptr)
	, m_pInvenSlotGroup(nullptr)
	, m_pInvenSlotSprites{}
	, m_pEquipSlotGroup(nullptr)
	, m_pEquipSlotStatics{}
	, m_iCurTab(InvenItemType::Equip)
{}


void UI_Inventory::onInit() {
	
	

	m_pInvenSlotGroup = CoreUIManager_v->getGroup(UI_INVENTORY_GROUP_INVEN_SLOTS);
	for (int i = 0; i < MAX_INVEN_SLOT_COUNT; ++i) {
		m_pInvenSlotSprites[i] = CoreUIManager_v->getSprite(UI_INVENTORY_INVEN_SLOTS_SPRITE_SLOT10 + i);
		m_pInvenSlotSprites[i]->setVisible(false);
	}

	m_pEquipSlotGroup = CoreUIManager_v->getGroup(UI_INVENTORY_GROUP_EQUIP_SLOTS);
	for (int i = 0; i < ItemType::MaxInvenEquip; ++i) {
		m_pEquipSlotStatics[i] = CoreUIManager_v->getStatic(UI_INVENTORY_EQUIP_SLOTS_STATIC_SHOULDER + i);
		m_pEquipSlotStatics[i]->setVisible(false);
	}

	m_pBackground = CoreUIManager_v->getSprite(UI_INVENTORY_SPRITE_BACKGROUND);
	m_pScrollBar = CoreUIManager_v->getScrollBar(UI_INVENTORY_SCROLLBAR);
	
}

void UI_Inventory::onLoaded() {
	m_pScrollBar->setRowCount(INVEN_ROW_COUNT);
	m_pScrollBar->setRowCountPerPage(INVEN_ROW_COUNT);
	m_pScrollBar->setLinkElement(m_pInvenSlotGroup);
}

void UI_Inventory::onAdded() {
	const int iRowCount = CoreInven_v->getAvailableSlotCount(m_iCurTab) / INVEN_ITEM_COUNT_PER_ROW;
	m_pScrollBar->setRowCount(iRowCount);
}

void UI_Inventory::onUpdate(float dt) {
}

bool UI_Inventory::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	return true;
}

bool UI_Inventory::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	return true;
}


