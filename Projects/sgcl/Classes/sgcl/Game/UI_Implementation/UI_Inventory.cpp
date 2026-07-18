#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_Inventory.h"

#include "sgcl/Game/Contents/UIManager.h"

USING_NS_CC;
USING_NS_JC;

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

void UI_Inventory::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	pInvenSlotGroup_ = FindElementByName<UIGroup>("groupInvenSlots");

	for (int i = 0; i < MAX_INVEN_SLOT_COUNT; ++i)
	{
		jc::String slotName = jc::StringUtil::Format("spriteSlot%d", i);
		pInvenSlotSprites_[i] = FindElementByName<UISprite>(slotName.Source());
		if (pInvenSlotSprites_[i])
			pInvenSlotSprites_[i]->setVisible(false);
	}

	pEquipSlotGroup_ = FindElementByName<UIGroup>("groupEquipSlots");

	for (int i = 0; i < ItemType::MaxInvenEquip; ++i)
	{
		jc::String staticName = jc::StringUtil::Format("staticEquip%d", i);
		pEquipSlotStatics_[i] = FindElementByName<UIStatic>(staticName.Source());
		if (pEquipSlotStatics_[i])
			pEquipSlotStatics_[i]->setVisible(false);
	}

	pBackground_ = FindElementByName<UISprite>("spriteBackground");
	pScrollBar_ = FindElementByName<UIScrollBar>("scrollbarInventory");
}

void UI_Inventory::OnLoaded()
{
	pScrollBar_->SetRowCount(INVEN_ROW_COUNT);
	pScrollBar_->SetRowCountPerPage(INVEN_ROW_COUNT);
	pScrollBar_->SetLinkElement(pInvenSlotGroup_);
}

void UI_Inventory::OnAdded()
{
	const int rowCount = sg::Contents.Inven->GetAvailableSlotCount(curTab_) / INVEN_ITEM_COUNT_PER_ROW;
	pScrollBar_->SetRowCount(rowCount);
}

void UI_Inventory::OnUpdate(float _dt)
{
}

bool UI_Inventory::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	return true;
}

bool UI_Inventory::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	return true;
}