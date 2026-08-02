/*
 * 작성자: 윤정도
 * =====================
 * 인벤토리 화면 (sgui 기반 재구현)
 */

#pragma once

#include "sgcl/Game/UI/Sgui.h"

#define MAX_INVEN_SLOT_COUNT		32
#define INVEN_ITEM_COUNT_PER_ROW	8
#define INVEN_ROW_COUNT				4

class UI_Inventory : public sgui::Window
{
public:
	virtual void OnInit(const jc::CDataMap<>& _param) override;
	virtual void OnLoaded() override;
	virtual void OnAdded() override;
	virtual bool OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode) override;
	virtual bool OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode) override;

private:
	void BuildEquipSlots(sgui::Panel* _container);
	void BuildInvenSlots(sgui::Panel* _container);

	sgui::Border* pBackground_ = nullptr;
	sgui::ScrollBar* pScrollBar_ = nullptr;

	sgui::Panel* pInvenSlotGrid_ = nullptr;
	sgui::Border* pInvenSlotBorders_[MAX_INVEN_SLOT_COUNT] = {};

	sgui::WrapPanel* pEquipSlotPanel_ = nullptr;
	sgui::Border* pEquipSlotBorders_[ItemType::MaxInvenEquip] = {};

	InvenItemType_t curTab_ = InvenItemType::Equip;
};
