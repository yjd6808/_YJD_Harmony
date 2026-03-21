/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:34 PM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "UI_Inventory.h"

#include "sgcl/Define_UI.h"
#include "sgcl/UIManager.h"

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UI_Inventory::UI_Inventory(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, pBackground_(nullptr)
, pScrollBar_(nullptr)
, pInvenSlotGroup_(nullptr)
, pInvenSlotSprites_{}
, pEquipSlotGroup_(nullptr)
, pEquipSlotStatics_{}
, curTab_(InvenItemType::Equip)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Inventory::OnInit()
{
	pInvenSlotGroup_ = g_cUIMgr.GetGroup(UI_INVENTORY_GROUP_INVEN_SLOTS);

	for (int i = 0; i < MAX_INVEN_SLOT_COUNT; ++i)
	{
		pInvenSlotSprites_[i] = g_cUIMgr.GetSprite(UI_INVENTORY_INVEN_SLOTS_SPRITE_SLOT10 + i);
		pInvenSlotSprites_[i]->setVisible(false);
	}

	pEquipSlotGroup_ = g_cUIMgr.GetGroup(UI_INVENTORY_GROUP_EQUIP_SLOTS);

	for (int i = 0; i < ItemType::MaxInvenEquip; ++i)
	{
		pEquipSlotStatics_[i] = g_cUIMgr.GetStatic(UI_INVENTORY_EQUIP_SLOTS_STATIC_SHOULDER + i);
		pEquipSlotStatics_[i]->setVisible(false);
	}

	pBackground_ = g_cUIMgr.GetSprite(UI_INVENTORY_SPRITE_BACKGROUND);
	pScrollBar_ = g_cUIMgr.GetScrollBar(UI_INVENTORY_SCROLLBAR);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Inventory::OnLoaded()
{
	pScrollBar_->SetRowCount(INVEN_ROW_COUNT);
	pScrollBar_->SetRowCountPerPage(INVEN_ROW_COUNT);
	pScrollBar_->SetLinkElement(pInvenSlotGroup_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Inventory::OnAdded()
{
	const int rowCount = sg::Contents.Inven->GetAvailableSlotCount(curTab_) / INVEN_ITEM_COUNT_PER_ROW;
	pScrollBar_->SetRowCount(rowCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Inventory::OnUpdate(float _dt)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UI_Inventory::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UI_Inventory::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	return true;
}
