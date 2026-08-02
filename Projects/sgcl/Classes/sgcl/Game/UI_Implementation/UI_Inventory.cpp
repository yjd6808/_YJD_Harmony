/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_Inventory.h"

#include "sgcl/Game/Contents/Contents.h"

USING_NS_CC;
USING_NS_JC;

using namespace sgui;

void UI_Inventory::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	SetDragMoveEnabled(true);

	// ==================== XML 우선 (layout/ui_inventory.xml 병합 시) ====================
	// sg-main 방식: XML로 로드된 Element를 OnInit 단에서 FindElementByName으로 멤버에 할당한다.
	// 슬롯들은 동적 데이터이므로 XML의 컨테이너 패널을 찾아 그 위에 생성한다.
	if (IsXmlLoaded())
	{
		pBackground_ = FindElementByName<Border>("background");
		pScrollBar_ = FindElementByName<ScrollBar>("스크롤바");

		if (Panel* pEquipContainer = FindElementByName<Panel>("equip_slots"))
		{
			BuildEquipSlots(pEquipContainer);
		}

		if (Panel* pInvenContainer = FindElementByName<Panel>("inven_slots"))
		{
			pInvenSlotGrid_ = pInvenContainer;
			BuildInvenSlots(pInvenContainer);
		}

		_LogDebug_("[UI_Inventory] OnInit(XML) background=%p scrollbar=%p equip=%p inven=%p",
			pBackground_, pScrollBar_, pEquipSlotPanel_, pInvenSlotGrid_);
		return;
	}

	// ==================== 코드 빌드 폴백 (XML 없음) ====================
	// 장비 슬롯
	pEquipSlotPanel_ = WrapPanel::Create(Orientation::Horizontal);
	pEquipSlotPanel_->setName("equip_slots");
	BuildEquipSlots(pEquipSlotPanel_);

	// 인벤 슬롯
	pInvenSlotGrid_ = UniformGrid::Create(INVEN_ROW_COUNT, INVEN_ITEM_COUNT_PER_ROW);
	pInvenSlotGrid_->setName("inven_slots");
	BuildInvenSlots(pInvenSlotGrid_);
	pInvenSlotGrid_->SetWidth(INVEN_ITEM_COUNT_PER_ROW * 44.0f);
	pInvenSlotGrid_->SetHeight(INVEN_ROW_COUNT * 44.0f);

	// 스크롤바
	pScrollBar_ = ScrollBar::Create();
	pScrollBar_->setName("스크롤바");
	pScrollBar_->SetHeight(INVEN_ROW_COUNT * 44.0f);

	// 배치
	StackPanel* pInvenArea = StackPanel::Create(Orientation::Horizontal);
	pInvenArea->SetSpacing(4.0f);
	pInvenArea->AddChild(pInvenSlotGrid_);
	pInvenArea->AddChild(pScrollBar_);

	StackPanel* pRootStack = StackPanel::Create(Orientation::Vertical);
	pRootStack->SetSpacing(12.0f);
	pRootStack->AddChild(pEquipSlotPanel_);
	pRootStack->AddChild(pInvenArea);

	pBackground_ = Border::Create();
	pBackground_->setName("background");
	pBackground_->SetBackground(SolidColorBrush::Create(0.05f, 0.05f, 0.08f, 0.9f));
	pBackground_->SetBorderBrush(UIColorF{ 0.35f, 0.35f, 0.4f, 1.0f });
	pBackground_->SetBorderThickness(Thickness(1.0f));
	pBackground_->SetPadding(Thickness(12.0f));
	pBackground_->SetHorizontalAlignment(HorizontalAlignment::Center);
	pBackground_->SetVerticalAlignment(VerticalAlignment::Center);
	pBackground_->SetChild(pRootStack);
	AddChild(pBackground_);
}

void UI_Inventory::BuildEquipSlots(sgui::Panel* _container)
{
	const char* equipSlotNames[] = { "shoulder", "top", "bottom", "shoes", "belt", "ring", "neck", "bracelt", "weapon", "title" };
	for (int idx = 0; idx < ItemType::MaxInvenEquip; ++idx)
	{
		pEquipSlotBorders_[idx] = Border::Create();
		pEquipSlotBorders_[idx]->setName(equipSlotNames[idx]);
		pEquipSlotBorders_[idx]->SetBackground(SolidColorBrush::Create(1.0f, 1.0f, 1.0f, 0.08f));
		pEquipSlotBorders_[idx]->SetBorderBrush(UIColorF{ 0.5f, 0.5f, 0.5f, 1.0f });
		pEquipSlotBorders_[idx]->SetBorderThickness(Thickness(1.0f));
		pEquipSlotBorders_[idx]->SetWidth(40.0f);
		pEquipSlotBorders_[idx]->SetHeight(40.0f);
		pEquipSlotBorders_[idx]->SetMargin(Thickness(2.0f));
		pEquipSlotBorders_[idx]->SetVisibility(Visibility::Hidden);
		_container->AddChild(pEquipSlotBorders_[idx]);
	}
}

void UI_Inventory::BuildInvenSlots(sgui::Panel* _container)
{
	for (int idx = 0; idx < MAX_INVEN_SLOT_COUNT; ++idx)
	{
		jc::String slotName = jc::StringUtil::Format("slot%d%d", idx / INVEN_ITEM_COUNT_PER_ROW + 1, idx % INVEN_ITEM_COUNT_PER_ROW);
		pInvenSlotBorders_[idx] = Border::Create();
		pInvenSlotBorders_[idx]->setName(slotName.Source());
		pInvenSlotBorders_[idx]->SetBackground(SolidColorBrush::Create(1.0f, 1.0f, 1.0f, 0.08f));
		pInvenSlotBorders_[idx]->SetBorderBrush(UIColorF{ 0.5f, 0.5f, 0.5f, 1.0f });
		pInvenSlotBorders_[idx]->SetBorderThickness(Thickness(1.0f));
		pInvenSlotBorders_[idx]->SetMargin(Thickness(2.0f));
		pInvenSlotBorders_[idx]->SetVisibility(Visibility::Hidden);
		_container->AddChild(pInvenSlotBorders_[idx]);
	}
}

void UI_Inventory::OnLoaded()
{
	if (!pScrollBar_ || !pInvenSlotGrid_)
	{
		return;
	}

	pScrollBar_->SetRowCount(INVEN_ROW_COUNT);
	pScrollBar_->SetRowCountPerPage(INVEN_ROW_COUNT);
	pScrollBar_->SetLinkElement(pInvenSlotGrid_);
}

void UI_Inventory::OnAdded()
{
	if (!pScrollBar_)
	{
		return;
	}

	const int rowCount = sg::Contents.Inven->GetAvailableSlotCount(curTab_) / INVEN_ITEM_COUNT_PER_ROW;
	pScrollBar_->SetRowCount(rowCount);
}

bool UI_Inventory::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode)
{
	UNUSED(_keyCode);
	return true;
}

bool UI_Inventory::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode)
{
	UNUSED(_keyCode);
	return true;
}

REGISTER_WINDOW(UI_Inventory, "ui_inventory")
