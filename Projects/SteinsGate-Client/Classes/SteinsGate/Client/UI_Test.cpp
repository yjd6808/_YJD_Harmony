/*
 * 작성자: 윤정도
 * 생성일: 4/27/2023 10:51:41 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UI_Test.h"

#include <SteinsGate/Client/Define_UI.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UI_Test::UI_Test(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, groupO1_(nullptr)
, groupO2_(nullptr)
, groupO1DefaultSize_()
, groupO2DefaultSize_()
, fontCode_(1)
, testLabelList_()
, testFont_(false)
, scale_(0.0f)
, scaleState_(eDecrease)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Test::OnInit()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Test::OnLoaded()
{
	// #define UI_TEST_TOGGLEBUTTON_T1	
	// #define UI_TEST_SPRITE_T2	
	// #define UI_TEST_CHECKBOX_T3	
	// #define UI_TEST_SCROLLBAR_T4	
	// #define UI_TEST_EDITBOX_T5	
	// #define UI_TEST_LABEL_T6	
	// #define UI_TEST_LABEL_FONT_TEST	
	// #define UI_TEST_LABEL_POPUP_TEST	
	// #define UI_TEST_LABEL_DRAG_TEST	
	// #define UI_TEST_LABEL_SCALE_TEST	
	// #define UI_TEST_LABEL_LINE_BREAK	
	// #define UI_TEST_GROUP_O1	
	// #define UI_TEST_O1_PROGRESSBAR_T1	
	// #define UI_TEST_O1_PROGRESSBAR_T2	
	// #define UI_TEST_O1_PROGRESSBAR_T3	
	// #define UI_TEST_O1_PROGRESSBAR_T4	
	// #define UI_TEST_GROUP_O2	
	// #define UI_TEST_O2_TOGGLEBUTTON_T1	
	// #define UI_TEST_O2_PROGRESSBAR_TOPBOTTOM	
	// #define UI_TEST_O2_SCROLLBAR_T3	
	// #define UI_TEST_O2_STATIC_T4	
	// #define UI_TEST_O2_CHECKBOX_T5	
	// #define UI_TEST_O2_LABEL_T6	
	// #define UI_TEST_O2_BUTTON_T7	
	// #define UI_TEST_O2_EDITBOX_T8	
	// #define UI_TEST_O2_GROUP_O21	
	// #define UI_TEST_O2_O21_TOGGLEBUTTON_T1	
	// #define UI_TEST_O2_O21_PROGRESSBAR_TOPBOTTOM	
	// #define UI_TEST_O2_O21_SCROLLBAR_T3	
	// #define UI_TEST_O2_O21_STATIC_T4	
	// #define UI_TEST_O2_O21_CHECKBOX_T5	
	// #define UI_TEST_O2_O21_LABEL_T6	
	// #define UI_TEST_O2_O21_BUTTON_T7	
	// #define UI_TEST_O2_O21_EDITBOX_T8	

	groupO1_ = Core::Contents.UIManager->getGroup(UI_TEST_GROUP_O1);
	groupO2_ = Core::Contents.UIManager->getGroup(UI_TEST_GROUP_O2);

	groupO1DefaultSize_ = groupO1_->getContentSize();
	groupO2DefaultSize_ = groupO2_->getContentSize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Test::onUpdate(float _dt)
{
	if (scaleState_ == eIncrease)
	{
		scale_ += _dt;

		if (scale_ > 1.25f)
			return;

		groupO1_->setContentSize(groupO1DefaultSize_ * scale_);
		groupO2_->setContentSize(groupO2DefaultSize_ * scale_);
	}
	else
	{
		scale_ -= _dt;

		if (scale_ < 1.0f)
			return;

		groupO1_->setContentSize(groupO1DefaultSize_ * scale_);
		groupO2_->setContentSize(groupO2DefaultSize_ * scale_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Test::OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Test::OnMouseUpTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent)
{
	CC_UNUSED_PARAM(_pMouseEvent);

	if (_pElement->GetCode() == UI_TEST_LABEL_POPUP_TEST)
	{
		Core::Contents.PopupManager->showOk(
			"안녕하세요\n안녕하세요2\n안녕하세요3\n안녕하세요안녕하세요안녕하세요안녕하세요안녕하세요안녕하세요"
		);
	}
	else if (_pElement->GetCode() == UI_TEST_LABEL_SCALE_TEST)
	{
		if (scaleState_ == eDecrease)
		{
			scale_ = 1.0f;
			scaleState_ = eIncrease;
		}
		else
		{
			scale_ = 1.25f;
			scaleState_ = eDecrease;
		}
	}
	else if (_pElement->GetCode() == UI_TEST_LABEL_FONT_TEST)
	{
		testFont_ = !testFont_;

		for (int i = 0; i < testLabelList_.Size(); ++i)
		{
			Core::Contents.World->removeChild(testLabelList_[i]);
		}
		testLabelList_.Clear();

		if (!testFont_)
			return;

		// 1: DF.ttf
		// 2: DM.ttf
		// 3: DMB.ttf
		// 4: DML.ttf
		// 5: N2G.ttf
		// 6: N2GB.ttf
		// 7: N2GM.ttf

		float height = 0.0f;
		const SGString fontName = Core::Contents.FontManager->getFontName(fontCode_);
		const SGString fontPath = Path::Combine(Core::CommonInfo->dataPath_, Const::Resource::FontDirName, fontName);
		std::string text;

		_LogDebug_("폰트이름: %s", fontName.Source());
		for (int i = 10; i <= 32; ++i)
		{
			text = StringUtils::format("안녕하세요 %d", i);
			auto pLabel = Label::createWithTTF(text, fontPath.Source(), i, Size::ZERO);
			auto pFontAtlas = pLabel->getFontAtlas();
			CC_UNUSED_PARAM(pFontAtlas);
			pLabel->setPosition(150.0f, height);
			pLabel->setAnchorPoint(Vec2::ZERO);
			_LogDebug_("txt: %s, lineHeight: %.f, width: %.f", text.data(), pLabel->getLineHeight(),
			           pLabel->getWidth());
			height += static_cast<float>(i);
			Core::Contents.World->addChild(pLabel);
			testLabelList_.PushBack(pLabel);
		}

		float fontSize = 14.0f;
		text = StringUtils::format("안녕하세요 1234567890123456789\n안녕하세요 1\n안녕하세요 1\n안녕하세요 1");
		auto pLabel = Label::createWithTTF(text, fontPath.Source(), fontSize, { 100, 0 });
		pLabel->setPosition(350.0f, 200.0f);
		pLabel->setAnchorPoint(Vec2::ZERO);
		pLabel->setLineHeight(fontSize);
		Core::Contents.World->addChild(pLabel);
		testLabelList_.PushBack(pLabel);

		fontCode_++;

		_LogDebug_("라인 너비 벡터 크기: %d", pLabel->getLinesWidth().size());
		_LogDebug_("라인 수: %d", pLabel->getStringNumLines());

		if (fontCode_ >= Core::Contents.FontManager->getFontCount())
		{
			fontCode_ = 1;
		}
	}
	else if (_pElement->GetCode() == UI_TEST_LABEL_DRAG_TEST)
	{
		forEachRecursiveContainedSelf([](UIElement* _pInnerElement)
		{
			_pInnerElement->SetDraggable(!_pInnerElement->IsDraggable());
		});

		Core::Contents.UIManager->getToggleButton(UI_TEST_TOGGLEBUTTON_T1)->SetDragLinkElement(this);
	}
	else if (_pElement->GetCode() == UI_TEST_LABEL_RESET_POSITION)
	{
		resetChildrenPosition();
	}
}
