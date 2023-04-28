/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:08 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIMasterGroup.h>

#define MAX_INVEN_SLOT_COUNT		32
#define INVEN_ITEM_COUNT_PER_ROW	8
#define INVEN_ROW_COUNT				4

class UI_Inventory : public UIMasterGroup
{
public:
	UI_Inventory(UIGroupInfo* groupInfo);

	void onInit() override;
	void onLoaded() override;
	void onAdded() override;
	void onUpdate(float dt) override;
	bool onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
	bool onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

private:
	UISprite* m_pBackground;
	UIScrollBar* m_pScrollBar;

	UIGroup* m_pInvenSlotGroup;
	UISprite* m_pInvenSlotSprites[MAX_INVEN_SLOT_COUNT];

	UIGroup* m_pEquipSlotGroup;
	UIStatic* m_pEquipSlotStatics[ItemType::MaxInvenEquip];

	InvenItemType_t m_iCurTab;
};


