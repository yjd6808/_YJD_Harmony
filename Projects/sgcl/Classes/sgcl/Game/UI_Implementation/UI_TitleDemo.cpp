/*
 * 작성자: 윤정도
 * =====================
 * WindowStyle:Title 데모 (타이틀바 + 캡션 버튼 + 최소화/최대화)
 */

#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_TitleDemo.h"

#include "sgcl/Game/UI/Controls/Button.h"
#include "sgcl/Game/UI/Controls/TextBlock.h"
#include "sgcl/Game/UI/Core/UIElement.h"

USING_NS_JC;

namespace
{
const char* StateName(sgui::Window* _pWindow)
{
	if (_pWindow->IsMinimized()) return "Minimized";
	if (_pWindow->IsMaximized()) return "Maximized";
	return "Normal";
}
}

void UI_TitleDemo::OnInit(const CDataMap<>& _param)
{
	Window::OnInit(_param);

	if (IsXmlLoaded())
	{
		pLabelState_ = FindName<sgui::TextBlock>("lbl_state");

		// 타이틀바 크롬 검증
		const bool chromeBuilt = GetWindowStyle() == sgui::WindowStyle::Title
			&& FindName("btn_window_minimize") != nullptr
			&& FindName("btn_window_maximize") != nullptr
			&& FindName("btn_window_close") != nullptr;

		_LogDebug_("[UI_TitleDemo] OnInit(XML) chrome=%d title=%s size=%.0fx%.0f",
			chromeBuilt ? 1 : 0, GetTitle(), GetWidth(), GetHeight());

		// 상태 전이 테스트 버튼
		sgui::Button* pBtnMinimize = FindName<sgui::Button>("btn_minimize");
		if (pBtnMinimize)
		{
			pBtnMinimize->SetClickCallback([this](sgui::ButtonBase*) { Minimize(); UpdateStateLabel(); });
		}

		sgui::Button* pBtnRestore = FindName<sgui::Button>("btn_restore");
		if (pBtnRestore)
		{
			pBtnRestore->SetClickCallback([this](sgui::ButtonBase*) { Restore(); UpdateStateLabel(); });
		}

		sgui::Button* pBtnToggleMax = FindName<sgui::Button>("btn_toggle_max");
		if (pBtnToggleMax)
		{
			pBtnToggleMax->SetClickCallback([this](sgui::ButtonBase*) { ToggleMaximize(); UpdateStateLabel(); });
		}

		sgui::Button* pBtnClose = FindName<sgui::Button>("btn_close");
		if (pBtnClose)
		{
			pBtnClose->SetClickCallback([this](sgui::ButtonBase*) { Close(); });
		}

		_LogDebug_("[UI_TitleDemo] caption buttons: min=%p max=%p close=%p",
			(void*)FindName("btn_window_minimize"), (void*)FindName("btn_window_maximize"), (void*)FindName("btn_window_close"));

		UpdateStateLabel();
	}
}

void UI_TitleDemo::OnLoaded()
{
	_LogDebug_("[UI_TitleDemo] OnLoaded");

	scheduleOnce([this](float)
	{
		std::function<void(cc::Node*, int)> dump = [&dump](cc::Node* pNode, int depth) -> void
		{
			if (!pNode)
			{
				return;
			}

			cc::Sprite* pSprite = dynamic_cast<cc::Sprite*>(pNode);
			cc::Label* pLabel = dynamic_cast<cc::Label*>(pNode);
			cc::LayerColor* pLayer = dynamic_cast<cc::LayerColor*>(pNode);

			_LogDebug_("[UI_TitleDemo] dump %s%s z=%d pos=(%.1f,%.1f) cs=(%.1f,%.1f) vis=%d opa=%d%s%s%s",
				std::string(depth * 2, ' ').c_str(),
				pNode->getName().empty() ? "<unnamed>" : pNode->getName().c_str(),
				pNode->getLocalZOrder(),
				pNode->getPositionX(), pNode->getPositionY(),
				pNode->getContentSize().width, pNode->getContentSize().height,
				pNode->isVisible() ? 1 : 0,
				pNode->getOpacity(),
				pSprite ? " [Sprite]" : "",
				pLabel ? " [Label]" : "",
				pLayer ? " [LayerColor]" : "");

			const auto& children = pNode->getChildren();
			for (auto* pChild : children)
			{
				dump(pChild, depth + 1);
			}
		};

			dump(this, 0);
			_LogDebug_("[UI_TitleDemo] dump -- force dirty + sortAllChildren --");
			cc::Node* pProbe = cc::Node::create();
			pProbe->setName("probe");
			this->addChild(pProbe, 999);
			this->sortAllChildren();
			dump(this, 0);
	}, 1.0f, "dump");
}

void UI_TitleDemo::UpdateStateLabel()
{
	if (!pLabelState_)
	{
		return;
	}

	pLabelState_->SetText(jc::StringUtil::Format("state: %s (%.0fx%.0f)", StateName(this), GetWidth(), GetHeight()).Source());

	_LogDebug_("[UI_TitleDemo] state=%s size=%.0fx%.0f pos=(%.0f,%.0f)",
		StateName(this), GetWidth(), GetHeight(), getPositionX(), getPositionY());
}

REGISTER_WINDOW(UI_TitleDemo, "ui_title_demo")
