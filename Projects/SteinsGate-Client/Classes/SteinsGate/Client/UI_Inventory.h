/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:08 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIRootGroup.h>

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
	void onUpdate(float _dt) override;
	bool onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	bool onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;

private:
	UISprite* background_;
	UIScrollBar* scrollBar_;

	UIGroup* invenSlotGroup_;
	UISprite* invenSlotSprites_[MAX_INVEN_SLOT_COUNT];

	UIGroup* equipSlotGroup_;
	UIStatic* equipSlotStatics_[ItemType::MaxInvenEquip];

	InvenItemType_t curTab_;
};
