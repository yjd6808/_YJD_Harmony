/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:08 PM
 * =====================
 *
 */


#pragma once

#include "sgcl/Game/UI/UIRootGroup.h"

#define MAX_INVEN_SLOT_COUNT		32
#define INVEN_ITEM_COUNT_PER_ROW	8
#define INVEN_ROW_COUNT				4

class UI_Inventory : public UIRootGroup
{
public:
	UI_Inventory(UIGroupInfo* _pGroupInfo);

	void OnInit() override;
	void OnLoaded() override;
	void OnAdded() override;
	void OnUpdate(float _dt) override;
	bool OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	bool OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;

private:
	UISprite* pBackground_;
	UIScrollBar* pScrollBar_;

	UIGroup* pInvenSlotGroup_;
	UISprite* pInvenSlotSprites_[MAX_INVEN_SLOT_COUNT];

	UIGroup* pEquipSlotGroup_;
	UIStatic* pEquipSlotStatics_[ItemType::MaxInvenEquip];

	InvenItemType_t curTab_;
};
