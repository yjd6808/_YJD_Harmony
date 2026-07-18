#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_Test.h"

#include "sgcl/Scene/Scene_World.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Game/Contents/FontManager.h"

USING_NS_CC;
USING_NS_JC;

UI_Test::UI_Test(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, pGroup01_(nullptr)
, pGroup02_(nullptr)
, groupO1DefaultSize_()
, groupO2DefaultSize_()
, fontCode_(1)
, testLabelList_()
, testFont_(false)
, scale_(0.0f)
, scaleState_(eDecrease)
{
}

void UI_Test::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);
}

void UI_Test::OnLoaded()
{
	pGroup01_ = FindElementByName<UIGroup>("groupO1");
	pGroup02_ = FindElementByName<UIGroup>("groupO2");

	groupO1DefaultSize_ = pGroup01_->getContentSize();
	groupO2DefaultSize_ = pGroup02_->getContentSize();
}

void UI_Test::OnUpdate(float _dt)
{
	if (scaleState_ == eIncrease)
	{
		scale_ += _dt;

		if (scale_ > 1.25f)
			return;

		pGroup01_->setContentSize(groupO1DefaultSize_ * scale_);
		pGroup02_->setContentSize(groupO2DefaultSize_ * scale_);
	}
	else
	{
		scale_ -= _dt;

		if (scale_ < 1.0f)
			return;

		pGroup01_->setContentSize(groupO1DefaultSize_ * scale_);
		pGroup02_->setContentSize(groupO2DefaultSize_ * scale_);
	}
}

void UI_Test::OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state)
{
}

void UI_Test::OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent)
{
	CC_UNUSED_PARAM(_pMouseEvent);
	const char* name = _pElement->GetName();

	if (strcmp(name, "labelPopupTest") == 0)
	{
		sg::Contents.PopupManager->ShowOk(
			"안녕하세요\n안녕하세요2\n안녕하세요3\n안녕하세요안녕하세요안녕하세요안녕하세요안녕하세요안녕하세요"
		);
	}
	else if (strcmp(name, "labelScaleTest") == 0)
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
	else if (strcmp(name, "labelFontTest") == 0)
	{
		testFont_ = !testFont_;

		for (int i = 0; i < testLabelList_.Size(); ++i)
		{
			g_cWorld.removeChild(testLabelList_[i]);
		}
		testLabelList_.Clear();

		if (!testFont_)
			return;

		float height = 0.0f;
		const jc::String& fontName = g_cFontMgr.GetFontName(fontCode_);
		const jc::String fontPath = g_cFontMgr.GetFontPath(fontCode_);
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
			g_cWorld.addChild(pLabel);
			testLabelList_.PushBack(pLabel);
		}

		float fontSize = 14.0f;
		text = StringUtils::format("안녕하세요 1234567890123456789\n안녕하세요 1\n안녕하세요 1\n안녕하세요 1");
		auto pLabel = Label::createWithTTF(text, fontPath.Source(), fontSize, { 100, 0 });
		pLabel->setPosition(350.0f, 200.0f);
		pLabel->setAnchorPoint(Vec2::ZERO);
		pLabel->setLineHeight(fontSize);
		g_cWorld.addChild(pLabel);
		testLabelList_.PushBack(pLabel);

		fontCode_++;

		_LogDebug_("라인 너비 벡터 크기: %d", pLabel->getLinesWidth().size());
		_LogDebug_("라인 수: %d", pLabel->getStringNumLines());

		if (fontCode_ >= g_cFontMgr.GetFontCount())
		{
			fontCode_ = 1;
		}
	}
	else if (strcmp(name, "labelDragTest") == 0)
	{
		ForEachRecursiveContainedSelf([](UIElement* _pInnerElement)
		{
			_pInnerElement->SetDraggable(!_pInnerElement->IsDraggable());
		});
	}
	else if (strcmp(name, "labelResetPosition") == 0)
	{
		ResetChildrenPosition();
	}
}