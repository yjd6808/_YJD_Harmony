/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_Test.h"

#include "sgcl/Scene/Scene_World.h"
#include "sgcl/Game/Contents/Contents.h"
#include "sgcl/Game/Contents/PopupManager.h"
#include "sgcl/Game/Contents/FontManager.h"

USING_NS_CC;
USING_NS_JC;

using namespace sgui;

void UI_Test::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	// ==================== XML 우선 (layout/ui_test.xml 병합 시) ====================
	// sg-main 방식: XML로 로드된 Element를 OnInit 단에서 FindElementByName으로 멤버에 할당한다.
	if (IsXmlLoaded())
	{
		pGroup01_ = FindElementByName<Border>("groupO1");
		pGroup02_ = FindElementByName<Border>("groupO2");

		if (Button* pBtn = FindElementByName<Button>("labelPopupTest"))
		{
			pBtn->SetClickCallback([this](ButtonBase*) { OnPopupTest(); });
		}
		if (Button* pBtn = FindElementByName<Button>("labelScaleTest"))
		{
			pBtn->SetClickCallback([this](ButtonBase*) { OnScaleTest(); });
		}
		if (Button* pBtn = FindElementByName<Button>("labelFontTest"))
		{
			pBtn->SetClickCallback([this](ButtonBase*) { OnFontTest(); });
		}
		if (Button* pBtn = FindElementByName<Button>("labelDragTest"))
		{
			pBtn->SetClickCallback([this](ButtonBase*) { OnDragTest(); });
		}
		if (Button* pBtn = FindElementByName<Button>("labelResetPosition"))
		{
			pBtn->SetClickCallback([this](ButtonBase*) { CenterOnHost(); });
		}

		// ==================== 표현식 바인딩 검증 로그 ====================
		const auto logExprText = [this](const char* _name)
		{
			if (TextBlock* pText = FindElementByName<TextBlock>(_name))
			{
				_LogDebug_("[UI_Test] expr %s = [%s]", _name, pText->GetText().c_str());
			}
		};

		logExprText("expr_arith");
		logExprText("expr_paren");
		logExprText("expr_mod");
		logExprText("expr_data");
		logExprText("expr_hex");
		logExprText("expr_bin");
		logExprText("expr_float");
		logExprText("expr_double");
		logExprText("expr_map");
		logExprText("expr_nestedmap");
		logExprText("expr_array");
		logExprText("expr_nestedidx");
		logExprText("expr_str");
		logExprText("expr_cmp");
		logExprText("expr_logic");
		logExprText("expr_max");
		logExprText("expr_min");
		logExprText("expr_elem");
		logExprText("expr_elemchain");
		logExprText("expr_elemexpr");
		logExprText("expr_parent");
		logExprText("expr_root");
		logExprText("expr_interp");
		logExprText("expr_empty");

		if (ProgressBar* pPbar = FindElementByName<ProgressBar>("pbar_percent"))
		{
			_LogDebug_("[UI_Test] expr pbar_percent=%.1f", pPbar->GetPercent());
		}
		if (ToggleButton* pChk = FindElementByName<ToggleButton>("chk_bind"))
		{
			_LogDebug_("[UI_Test] expr chk_bind=%d", pChk->IsChecked() ? 1 : 0);
		}
		if (ToggleButton* pChk = FindElementByName<ToggleButton>("chk_bool"))
		{
			_LogDebug_("[UI_Test] expr chk_bool=%d", pChk->IsChecked() ? 1 : 0);
		}
		if (Border* pGroup = FindElementByName<Border>("groupO2"))
		{
			_LogDebug_("[UI_Test] expr groupO2 width=%.1f height=%.1f", pGroup->GetWidth(), pGroup->GetHeight());
		}

		_LogDebug_("[UI_Test] OnInit(XML) groupO1=%p groupO2=%p", pGroup01_, pGroup02_);
		return;
	}

	// ==================== 코드 빌드 폴백 (XML 없음) ====================
	StackPanel* pMenuStack = StackPanel::Create(Orientation::Vertical);
	pMenuStack->SetSpacing(6.0f);
	pMenuStack->SetHorizontalAlignment(HorizontalAlignment::Left);
	pMenuStack->SetVerticalAlignment(VerticalAlignment::Top);
	pMenuStack->SetMargin(Thickness(12.0f));

	Button* pBtnPopupTest = Button::Create("팝업 테스트");
	pBtnPopupTest->setName("labelPopupTest");
	pBtnPopupTest->SetClickCallback([this](ButtonBase*) { OnPopupTest(); });
	pMenuStack->AddChild(pBtnPopupTest);

	Button* pBtnScaleTest = Button::Create("스케일 테스트");
	pBtnScaleTest->setName("labelScaleTest");
	pBtnScaleTest->SetClickCallback([this](ButtonBase*) { OnScaleTest(); });
	pMenuStack->AddChild(pBtnScaleTest);

	Button* pBtnFontTest = Button::Create("폰트 테스트");
	pBtnFontTest->setName("labelFontTest");
	pBtnFontTest->SetClickCallback([this](ButtonBase*) { OnFontTest(); });
	pMenuStack->AddChild(pBtnFontTest);

	Button* pBtnDragTest = Button::Create("드래그 테스트");
	pBtnDragTest->setName("labelDragTest");
	pBtnDragTest->SetClickCallback([this](ButtonBase*) { OnDragTest(); });
	pMenuStack->AddChild(pBtnDragTest);

	Button* pBtnResetPosition = Button::Create("위치 초기화");
	pBtnResetPosition->setName("labelResetPosition");
	pBtnResetPosition->SetClickCallback([this](ButtonBase*) { CenterOnHost(); });
	pMenuStack->AddChild(pBtnResetPosition);

	AddChild(pMenuStack);

	// 스케일 테스트용 그룹 2개
	pGroup01_ = Border::Create();
	pGroup01_->setName("groupO1");
	pGroup01_->SetBackground(SolidColorBrush::Create(0.2f, 0.4f, 0.8f, 0.5f));
	pGroup01_->SetHorizontalAlignment(HorizontalAlignment::Left);
	pGroup01_->SetVerticalAlignment(VerticalAlignment::Top);
	pGroup01_->SetWidth(160.0f);
	pGroup01_->SetHeight(120.0f);
	pGroup01_->SetMargin(Thickness(220.0f, 12.0f, 0.0f, 0.0f));
	AddChild(pGroup01_);

	pGroup02_ = Border::Create();
	pGroup02_->setName("groupO2");
	pGroup02_->SetBackground(SolidColorBrush::Create(0.8f, 0.4f, 0.2f, 0.5f));
	pGroup02_->SetHorizontalAlignment(HorizontalAlignment::Left);
	pGroup02_->SetVerticalAlignment(VerticalAlignment::Top);
	pGroup02_->SetWidth(160.0f);
	pGroup02_->SetHeight(120.0f);
	pGroup02_->SetMargin(Thickness(220.0f, 150.0f, 0.0f, 0.0f));
	AddChild(pGroup02_);
}

void UI_Test::OnLoaded()
{
	if (!pGroup01_ || !pGroup02_)
	{
		return;
	}

	groupO1DefaultSize_ = cc::size(pGroup01_->GetWidth(), pGroup01_->GetHeight());
	groupO2DefaultSize_ = cc::size(pGroup02_->GetWidth(), pGroup02_->GetHeight());
}

void UI_Test::OnUpdate(float _dt)
{
	if (!pGroup01_ || !pGroup02_)
	{
		return;
	}

	if (scaleState_ == eIncrease)
	{
		scale_ += _dt;

		if (scale_ > 1.25f)
			return;
	}
	else
	{
		scale_ -= _dt;

		if (scale_ < 1.0f)
			return;
	}

	pGroup01_->SetWidth(groupO1DefaultSize_.width * scale_);
	pGroup01_->SetHeight(groupO1DefaultSize_.height * scale_);
	pGroup02_->SetWidth(groupO2DefaultSize_.width * scale_);
	pGroup02_->SetHeight(groupO2DefaultSize_.height * scale_);
}

void UI_Test::OnPopupTest()
{
	sg::Contents.PopupManager->ShowOk(
		"안녕하세요\n안녕하세요2\n안녕하세요3\n안녕하세요안녕하세요안녕하세요안녕하세요안녕하세요안녕하세요"
	);
}

void UI_Test::OnScaleTest()
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

void UI_Test::OnFontTest()
{
	testFont_ = !testFont_;

	for (int idx = 0; idx < testLabelList_.Size(); ++idx)
	{
		g_cWorld.removeChild(testLabelList_[idx]);
	}
	testLabelList_.Clear();

	if (!testFont_)
		return;

	float height = 0.0f;
	const jc::String& fontName = g_cFontMgr.GetFontName(fontCode_);
	const jc::String fontPath = g_cFontMgr.GetFontPath(fontCode_);
	std::string text;

	_LogDebug_("폰트이름: %s", fontName.Source());
	for (int idx = 10; idx <= 32; ++idx)
	{
		text = StringUtils::format("안녕하세요 %d", idx);
		auto pLabel = Label::createWithTTF(text, fontPath.Source(), idx, Size::ZERO);
		auto pFontAtlas = pLabel->getFontAtlas();
		CC_UNUSED_PARAM(pFontAtlas);
		pLabel->setPosition(150.0f, height);
		pLabel->setAnchorPoint(Vec2::ZERO);
		_LogDebug_("txt: %s, lineHeight: %.f, width: %.f", text.data(), pLabel->getLineHeight(),
		           pLabel->getWidth());
		height += static_cast<float>(idx);
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

void UI_Test::OnDragTest()
{
	SetDragMoveEnabled(!IsDragMoveEnabled());
}

REGISTER_WINDOW(UI_Test, "ui_test")
