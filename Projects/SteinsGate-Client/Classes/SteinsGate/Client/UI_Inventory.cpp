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

//////////////////////////////////////////////////////////////////////////////////////////
UI_Inventory::UI_Inventory(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, background_(nullptr)
, scrollBar_(nullptr)
, invenSlotGroup_(nullptr)
, invenSlotSprites_{}
, equipSlotGroup_(nullptr)
, equipSlotStatics_{}
, curTab_(InvenItemType::Equip)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Inventory::OnInit()
{
	invenSlotGroup_ = Core::Contents.UIManager->getGroup(UI_INVENTORY_GROUP_INVEN_SLOTS);

	for (int i = 0; i < MAX_INVEN_SLOT_COUNT; ++i)
	{
		invenSlotSprites_[i] = Core::Contents.UIManager->getSprite(UI_INVENTORY_INVEN_SLOTS_SPRITE_SLOT10 + i);
		invenSlotSprites_[i]->setVisible(false);
	}

	equipSlotGroup_ = Core::Contents.UIManager->getGroup(UI_INVENTORY_GROUP_EQUIP_SLOTS);

	for (int i = 0; i < ItemType::MaxInvenEquip; ++i)
	{
		equipSlotStatics_[i] = Core::Contents.UIManager->getStatic(UI_INVENTORY_EQUIP_SLOTS_STATIC_SHOULDER + i);
		equipSlotStatics_[i]->setVisible(false);
	}

	background_ = Core::Contents.UIManager->getSprite(UI_INVENTORY_SPRITE_BACKGROUND);
	scrollBar_ = Core::Contents.UIManager->getScrollBar(UI_INVENTORY_SCROLLBAR);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Inventory::OnLoaded()
{
	scrollBar_->setRowCount(INVEN_ROW_COUNT);
	scrollBar_->setRowCountPerPage(INVEN_ROW_COUNT);
	scrollBar_->setLinkElement(invenSlotGroup_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Inventory::OnAdded()
{
	const int rowCount = Core::Contents.Inven->getAvailableSlotCount(curTab_) / INVEN_ITEM_COUNT_PER_ROW;
	scrollBar_->setRowCount(rowCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Inventory::onUpdate(float _dt)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UI_Inventory::onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UI_Inventory::onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	return true;
}
