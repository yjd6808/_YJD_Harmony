/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/Window.h"
#include "sgcl/Game/UI/Host/UIHost.h"
#include "sgcl/Game/UI/Controls/Border.h"
#include "sgcl/Game/UI/Controls/Button.h"
#include "sgcl/Game/UI/Controls/TextBlock.h"
#include "sgcl/Game/UI/Core/Brush.h"
#include "sgcl/Game/UI/Core/BrushVisual.h"
#include "sgcl/Game/UI/Panels/Grid.h"
#include "sgcl/Game/UI/Panels/StackPanel.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"
#include "jc/FileSystem/Path.h"
#include "jc/FileSystem/File.h"

namespace sgui
{

namespace
{
const float kTitleBarBorder_v = 1.0f;	// 타이틀바 하단 구분선 두께
}

Window::Window()
{
	// 윈도우는 기본적으로 호스트 전체를 덮는다.
	SetHorizontalAlignment(HorizontalAlignment::Stretch);
	SetVerticalAlignment(VerticalAlignment::Stretch);
}

Window::~Window()
{
	for (int idx = 0; idx < iconVisuals_.Size(); ++idx)
	{
		delete iconVisuals_[idx];
	}
	iconVisuals_.Clear();

	_LogDebug_("[Window] ~Window name=%s rc=%d running=%d", getName().c_str(), getReferenceCount(), isRunning() ? 1 : 0);
}

// ==================== 스타일 ====================
void Window::SetWindowStyle(WindowStyle _style)
{
	if (windowStyle_ == _style)
	{
		return;
	}

	windowStyle_ = _style;

	if (_style == WindowStyle::Title)
	{
		BuildTitleBarChrome();
	}
	else
	{
		DestroyTitleBarChrome();
	}

	MarkLayoutDirty();
}

void Window::SetTitle(const char* _title)
{
	title_ = _title ? _title : "";

	if (pTitleLabel_)
	{
		pTitleLabel_->SetText(title_.Source());
	}

	_LogDebug_("[Window] %s title=%s", getName().c_str(), title_.Source());
}

void Window::SetContentHost(UIElement* _pHost)
{
	if (_pHost == nullptr)
	{
		return;
	}

	pContentHost_ = _pHost;

	// Title 스타일: 레이아웃 그리드 1행(콘텐츠 영역)에 부착.
	// (WindowStyle 속성이 자식 파싱보다 먼저 적용되므로 chrome이 이미 존재한다.)
	if (windowStyle_ == WindowStyle::Title && pLayoutGrid_)
	{
		_pHost->SetGridRow(1);
		pLayoutGrid_->AddChild(_pHost);
	}
	else
	{
		AddChild(_pHost);
	}

	_LogDebug_("[Window] %s content host=%s", getName().c_str(), _pHost->getName().c_str());
}

void Window::SetTitleBarHeight(float _height)
{
	titleBarHeight_ = jc::Math::Max(20.0f, _height);

	if (pTitleBar_)
	{
		pTitleBar_->SetHeight(titleBarHeight_);
		MarkLayoutDirty();
	}
}

void Window::SetTitleBarBackground(const UIColorF& _color)
{
	titleBarBackground_ = _color;
	customTitleBarBackground_ = true;
	ApplyChromeColors();
}

void Window::SetTitleBarForeground(const UIColorF& _color)
{
	titleBarForeground_ = _color;

	if (pTitleLabel_)
	{
		pTitleLabel_->SetForeground(_color);
	}
}

void Window::SetWindowBackground(const UIColorF& _color)
{
	windowBackground_ = _color;
	customWindowBackground_ = true;
	ApplyChromeColors();
}

void Window::SetWindowBorderBrush(const UIColorF& _color)
{
	windowBorderBrush_ = _color;
	customWindowBorderBrush_ = true;
	ApplyChromeColors();
}

// ==================== 최소화/최대화 ====================
void Window::Minimize()
{
	if (state_ == WindowState::Minimized)
	{
		return;
	}

	if (state_ == WindowState::Normal)
	{
		SaveNormalRect();
	}

	state_ = WindowState::Minimized;

	if (pContentHost_)
	{
		pContentHost_->SetVisibility(Visibility::Collapsed);
	}

	// 타이틀바 + 창 테두리만 남긴다. (두께는 활성 테마의 window.borderWidth)
	const float windowBorder = UIThemeManager::Get()->GetActiveTheme().window.borderWidth;
	SetHeight(titleBarHeight_ + windowBorder * 2.0f);
	MarkLayoutDirty();

	_LogDebug_("[Window] %s minimized (height=%.0f)", getName().c_str(), GetHeight());
}

void Window::Maximize()
{
	if (state_ == WindowState::Maximized)
	{
		return;
	}

	if (state_ == WindowState::Normal)
	{
		SaveNormalRect();
	}

	state_ = WindowState::Maximized;

	if (pContentHost_)
	{
		pContentHost_->SetVisibility(Visibility::Visible);
	}

	// 호스트 전체 크기로 확장
	useExplicitPosition_ = false;
	SetHorizontalAlignment(HorizontalAlignment::Stretch);
	SetVerticalAlignment(VerticalAlignment::Stretch);
	SetWidth(AutoSize_v);
	SetHeight(AutoSize_v);
	MarkLayoutDirty();

	_LogDebug_("[Window] %s maximized", getName().c_str());
}

void Window::Restore()
{
	if (state_ == WindowState::Normal)
	{
		return;
	}

	state_ = WindowState::Normal;

	if (pContentHost_)
	{
		pContentHost_->SetVisibility(Visibility::Visible);
	}

	if (hasSavedRect_)
	{
		SetWindowPosition(savedX_, savedY_);
		SetWidth(savedWidth_);
		SetHeight(savedHeight_);
	}
	else
	{
		CenterOnHost();
	}

	MarkLayoutDirty();

	_LogDebug_("[Window] %s restored (pos=(%.0f,%.0f) size=(%.0fx%.0f))",
		getName().c_str(), explicitPosition_.x, explicitPosition_.y, GetWidth(), GetHeight());
}

void Window::ToggleMaximize()
{
	if (state_ == WindowState::Maximized)
	{
		Restore();
	}
	else
	{
		Maximize();
	}
}

void Window::SaveNormalRect()
{
	const float hostHeight = pHost_ ? pHost_->getContentSize().height : 0.0f;

	if (useExplicitPosition_)
	{
		savedX_ = explicitPosition_.x;
		savedY_ = explicitPosition_.y;
	}
	else
	{
		// 현재 렌더 위치를 명시 위치(y-down)로 환산한다.
		savedX_ = getPositionX();
		savedY_ = hostHeight - getPositionY() - renderSize_.height;
	}

	savedWidth_ = renderSize_.width;
	savedHeight_ = renderSize_.height;
	hasSavedRect_ = true;
}

// ==================== 타이틀바 크롬 ====================
void Window::BuildTitleBarChrome()
{
	if (pChrome_)
	{
		return;
	}

	// 외곽 Border: 창 배경 + 테두리 (두께는 테마 window.borderWidth)
	pChrome_ = Border::Create();
	pChrome_->SetBorderThickness(Thickness(UIThemeManager::Get()->GetActiveTheme().window.borderWidth));
	AddChild(pChrome_);

	// 레이아웃 Grid (2행: Auto 타이틀바 / Star 콘텐츠)
	pLayoutGrid_ = Grid::Create();
	pLayoutGrid_->AddRowDefinition(GridLength::Auto());
	pLayoutGrid_->AddRowDefinition(GridLength::Star(1.0f));
	pChrome_->SetChild(pLayoutGrid_);

	// 타이틀바 (하단 구분선 포함)
	pTitleBar_ = Border::Create();
	pTitleBar_->SetHeight(titleBarHeight_);
	pTitleBar_->SetBorderThickness(Thickness(0.0f, 0.0f, 0.0f, kTitleBarBorder_v));
	pTitleBarGrid_ = Grid::Create();
	pTitleBarGrid_->AddColumnDefinition(GridLength::Auto());
	pTitleBarGrid_->AddColumnDefinition(GridLength::Star(1.0f));
	pTitleBarGrid_->AddColumnDefinition(GridLength::Auto());
	pTitleBar_->SetChild(pTitleBarGrid_);
	pLayoutGrid_->AddChild(pTitleBar_);

	// 타이틀 텍스트 (좌측)
	pTitleLabel_ = TextBlock::Create(title_.Source());
	pTitleLabel_->SetMargin(Thickness(10.0f, 0.0f, 8.0f, 0.0f));
	pTitleLabel_->SetGridColumn(0);
	pTitleLabel_->SetForeground(titleBarForeground_);
	pTitleBarGrid_->AddChild(pTitleLabel_);

	// 캡션 버튼 (우측)
	pCaptionButtonStack_ = StackPanel::Create(Orientation::Horizontal);
	pCaptionButtonStack_->SetGridColumn(2);
	pTitleBarGrid_->AddChild(pCaptionButtonStack_);

	pBtnMinimize_ = BuildCaptionButton(UIAssetSemantic::WindowIconMinimize, "btn_window_minimize", "-");
	pBtnMaximize_ = BuildCaptionButton(UIAssetSemantic::WindowIconMaximize, "btn_window_maximize", "[]");
	pBtnClose_ = BuildCaptionButton(UIAssetSemantic::WindowIconClose, "btn_window_close", "X");

	// 드래그 이동: 타이틀바 빈 영역에서만 (버튼 클릭은 handled 상태라 무시됨)
	pTitleBar_->AddHandler(ueMouseDown, [this](UIElement*, UIEvent* _pEvent)
	{
		if (_pEvent->handled_)
		{
			return;
		}

		dragMoveActive_ = true;
		dragMoveLast_ = _pEvent->Cast<UIEventMouse>()->screenPos_;
		pTitleBar_->CaptureMouse();
		_pEvent->handled_ = true;
	});

	pTitleBar_->AddHandler(ueMouseMove, [this](UIElement*, UIEvent* _pEvent)
	{
		if (!dragMoveActive_)
		{
			return;
		}

		auto* pMouseEvent = _pEvent->Cast<UIEventMouse>();
		// 월드 좌표는 y-up이므로 y-down 이동량으로 변환해 적용한다.
		MoveWindowBy(pMouseEvent->screenPos_.x - dragMoveLast_.x, dragMoveLast_.y - pMouseEvent->screenPos_.y);
		dragMoveLast_ = pMouseEvent->screenPos_;
		_pEvent->handled_ = true;
	});

	pTitleBar_->AddHandler(ueMouseUp, [this](UIElement*, UIEvent* _pEvent)
	{
		if (!dragMoveActive_)
		{
			return;
		}

		dragMoveActive_ = false;
		pTitleBar_->ReleaseMouseCapture();
		_pEvent->handled_ = true;
	});

	ApplyChromeColors();
	SyncChromeSize();

	_LogDebug_("[Window] %s title bar chrome built", getName().c_str());
}

void Window::DestroyTitleBarChrome()
{
	for (int idx = 0; idx < iconVisuals_.Size(); ++idx)
	{
		delete iconVisuals_[idx];
	}
	iconVisuals_.Clear();

	pBtnMinimize_ = nullptr;
	pBtnMaximize_ = nullptr;
	pBtnClose_ = nullptr;
	pCaptionButtonStack_ = nullptr;
	pTitleLabel_ = nullptr;
	pTitleBarGrid_ = nullptr;
	pTitleBar_ = nullptr;
	pContentHost_ = nullptr;
	pLayoutGrid_ = nullptr;

	if (pChrome_)
	{
		pChrome_->removeFromParent();
		pChrome_ = nullptr;
	}
}

Button* Window::BuildCaptionButton(UIAssetSemantic _semantic, const char* _name, const char* _fallbackText)
{
	Button* pBtn = Button::Create(_fallbackText);
	pBtn->setName(_name);
	pBtn->SetWidth(28.0f);
	pBtn->SetHeight(28.0f);
	pBtn->SetMargin(Thickness(1.0f));
	// 28x28 캡션 버튼에서 기본 패딩(12,6)은 콘텐츠 슬롯을 (4,16)으로 줄여
	// 폴백 텍스트가 우측/하단으로 밀린다. 아이콘/텍스트는 버튼 중앙에 정렬한다.
	pBtn->SetPadding(Thickness(0.0f, 0.0f));
	pBtn->SetClickCallback([this, _semantic](ButtonBase*)
	{
		switch (_semantic)
		{
		case UIAssetSemantic::WindowIconMinimize: Minimize(); break;
		case UIAssetSemantic::WindowIconMaximize: ToggleMaximize(); break;
		case UIAssetSemantic::WindowIconClose: Close(); break;
		default: break;
		}
	});
	pCaptionButtonStack_->AddChild(pBtn);

	// SVG 아이콘 (bake 요소): 파일이 있으면 텍스트 대신 아이콘 스프라이트 표시
	const char* svgName = nullptr;
	switch (_semantic)
	{
	case UIAssetSemantic::WindowIconMinimize: svgName = "window-minimize.svg"; break;
	case UIAssetSemantic::WindowIconMaximize: svgName = "window-maximize.svg"; break;
	case UIAssetSemantic::WindowIconClose: svgName = "window-close.svg"; break;
	default: break;
	}

	UIThemeManager* pThemeMgr = UIThemeManager::Get();
	if (pThemeMgr && svgName)
	{
		const jc::String svgPath = jc::Path::Combine(pThemeMgr->GetBakeService().GetIconDirectory().Source(), svgName);

		if (jc::File::Exist(svgPath.Source()))
		{
			// 아이콘은 버튼 콘텐츠(폴백 텍스트) 위에 표시된다.
			BrushVisual* pIcon = dbg_new BrushVisual;
			pIcon->SetBrush(ThemeBrush::Create(_semantic, UIElementType::Button, UIComponentSlot::Icon, true));
			pIcon->Attach(pBtn, 1);
			pIcon->Resize(cc::size(20.0f, 20.0f));
			iconVisuals_.PushBack(pIcon);
		}
		else
		{
			_LogWarn_("[Window] 캡션 버튼 아이콘 파일 없음(window=%s, button=%s): %s",
				getName().c_str(), _name, svgPath.Source());
		}
	}

	return pBtn;
}

void Window::ApplyChromeColors()
{
	if (!pChrome_)
	{
		return;
	}

	// 테마/스킴 전환 시에도 명시 설정 색은 유지된다. (기본값은 활성 테마의 window 토큰)
	const UIRuntimeTheme& theme = UIThemeManager::Get()->GetActiveTheme();

	pChrome_->SetBackground(SolidColorBrush::Create(customWindowBackground_ ? windowBackground_ : theme.window.windowBackground));
	pChrome_->SetBorderBrush(customWindowBorderBrush_ ? windowBorderBrush_ : theme.window.borderColor);
	pChrome_->SetBorderThickness(Thickness(theme.window.borderWidth));

	if (pTitleBar_)
	{
		pTitleBar_->SetBackground(SolidColorBrush::Create(customTitleBarBackground_ ? titleBarBackground_ : theme.window.titleBarBackground));
		pTitleBar_->SetBorderBrush(customWindowBorderBrush_ ? windowBorderBrush_ : theme.window.borderColor);
	}

	if (pTitleLabel_)
	{
		pTitleLabel_->SetForeground(theme.window.titleBarForeground);
	}
}

void Window::RefreshThemeVisuals()
{
	// 타이틀바/창 배경은 현재 스킴 톤으로 갱신 (아이콘은 UIThemeBinding이 자동 교체)
	ApplyChromeColors();

	Panel::RefreshThemeVisuals();
}

void Window::SyncChromeSize()
{
	if (pChrome_)
	{
		pChrome_->SetWidth(renderSize_.width);
		pChrome_->SetHeight(renderSize_.height);
	}
}

void Window::OnRenderSizeChanged(const cc::size& _size)
{
	Panel::OnRenderSizeChanged(_size);
	SyncChromeSize();
}

// ==================== 이벤트 ====================
void Window::OnEventRaised(int _id, UIEvent* _pEvent)
{
	Panel::OnEventRaised(_id, _pEvent);

	switch (_pEvent->type_)
	{
	case ueKeyDown:
	{
		if (_pEvent->phase_ != UIRoutingPhase::Bubble || _pEvent->handled_)
			break;

		auto* pKeyEvent = _pEvent->Cast<UIEventKeyboard>();

		// 파생 클래스의 키 처리가 우선한다. (예: UI_Popup은 ESC에서 풀 반환 경로로 닫혀야 함)
		if (OnKeyPressed(pKeyEvent->keyCode_))
		{
			_pEvent->handled_ = true;
			break;
		}

		if (closeWithEscape_ && pKeyEvent->keyCode_ == cc::EventKeyboard::KeyCode::KEY_ESCAPE)
		{
			Close();
			_pEvent->handled_ = true;
		}
		break;
	}
	case ueKeyUp:
	{
		if (_pEvent->phase_ != UIRoutingPhase::Bubble || _pEvent->handled_)
			break;

		if (OnKeyReleased(_pEvent->Cast<UIEventKeyboard>()->keyCode_))
		{
			_pEvent->handled_ = true;
		}
		break;
	}
	case ueMouseDown:
	{
		// 빈 영역(자식 컨트롤이 처리하지 않은 곳)을 누르면 드래그 이동 시작
		if (dragMoveEnabled_ && _pEvent->phase_ == UIRoutingPhase::Bubble && !_pEvent->handled_)
		{
			dragMoveActive_ = true;
			dragMoveLast_ = _pEvent->Cast<UIEventMouse>()->screenPos_;
			CaptureMouse();
			_pEvent->handled_ = true;
		}
		break;
	}
	case ueMouseMove:
	{
		if (dragMoveActive_ && _pEvent->phase_ == UIRoutingPhase::Bubble)
		{
			auto* pMouseEvent = _pEvent->Cast<UIEventMouse>();
			// 월드 좌표는 y-up이므로 y-down 이동량으로 변환해 적용한다.
			MoveWindowBy(pMouseEvent->screenPos_.x - dragMoveLast_.x, dragMoveLast_.y - pMouseEvent->screenPos_.y);
			dragMoveLast_ = pMouseEvent->screenPos_;
			_pEvent->handled_ = true;
		}
		break;
	}
	case ueMouseUp:
	{
		if (dragMoveActive_ && _pEvent->phase_ == UIRoutingPhase::Bubble)
		{
			dragMoveActive_ = false;
			ReleaseMouseCapture();
			_pEvent->handled_ = true;
		}
		break;
	}
	default:
		break;
	}
}

void Window::Close()
{
	closeRequested_ = true;
}

void Window::SetWindowPosition(float _x, float _y)
{
	useExplicitPosition_ = true;
	explicitPosition_ = cc::vec2(_x, _y);
	SetHorizontalAlignment(HorizontalAlignment::Left);
	SetVerticalAlignment(VerticalAlignment::Top);
	MarkLayoutDirty();
}

void Window::CenterOnHost()
{
	useExplicitPosition_ = false;
	SetHorizontalAlignment(HorizontalAlignment::Center);
	SetVerticalAlignment(VerticalAlignment::Center);
	MarkLayoutDirty();
}

void Window::MoveWindowBy(float _deltaX, float _deltaY)
{
	if (!useExplicitPosition_)
	{
		// 현재 렌더 위치를 명시 위치로 전환한다. (y-up -> y-down 변환)
		const float hostHeight = pHost_ ? pHost_->getContentSize().height : 0.0f;
		explicitPosition_ = cc::vec2(getPositionX(), hostHeight - getPositionY() - renderSize_.height);
		useExplicitPosition_ = true;
		SetHorizontalAlignment(HorizontalAlignment::Left);
		SetVerticalAlignment(VerticalAlignment::Top);
	}

	explicitPosition_.x += _deltaX;
	explicitPosition_.y += _deltaY;
	MarkLayoutDirty();
}

UIElement* Window::FindName(const char* _name)
{
	if (_name == nullptr)
	{
		return nullptr;
	}

	UIElement* pFound = nullptr;

	ForEachRecursiveContainedSelf([&pFound, _name](UIElement* _pElement)
	{
		if (pFound == nullptr && _pElement->getName() == _name)
		{
			pFound = _pElement;
		}
	});

	return pFound;
}

void Window::FlushLayout(const cc::size& _hostSize)
{
	layoutDirty_ = false;

	Measure(_hostSize);

	// 호스트 전체 영역을 배치 기준으로 사용한다. (y-down)
	Arrange(Rect(0.0f, 0.0f, _hostSize.width, _hostSize.height));

	if (useExplicitPosition_)
	{
		// Arrange 결과 위치를 명시 위치로 덮어쓴다. (y-down -> y-up 변환)
		setPosition(explicitPosition_.x, _hostSize.height - explicitPosition_.y - renderSize_.height);
	}

	// 레이아웃 확정 후 호스트(버튼) 크기를 알 수 있으므로 아이콘을 호스트 중앙에 재배치한다.
	for (int idx = 0; idx < iconVisuals_.Size(); ++idx)
	{
		iconVisuals_[idx]->ApplySize();
	}
}

void Window::ArrangeOverride(const cc::size& _finalSize)
{
	// ContentHost(있는 경우)는 창 전체 영역을 채운다. (Title 스타일은 레이아웃 그리드 1행이 담당)
	if (pContentHost_)
	{
		pContentHost_->Arrange(Rect(0.0f, 0.0f, _finalSize.width, _finalSize.height));
	}

	// 나머지 자식은 Canvas 배치 규칙을 따른다.
	for (size_t i = 0; i < children_.size(); ++i)
	{
		UIElement* pChild = children_[i];

		if (pChild == pContentHost_)
		{
			continue;
		}

		const cc::size& desired = pChild->GetDesiredSize();

		float x = 0.0f;
		float y = 0.0f;

		if (!IsAutoSize(pChild->GetCanvasLeft()))
		{
			x = pChild->GetCanvasLeft();
		}
		else if (!IsAutoSize(pChild->GetCanvasRight()))
		{
			x = _finalSize.width - pChild->GetCanvasRight() - desired.width;
		}

		if (!IsAutoSize(pChild->GetCanvasTop()))
		{
			y = pChild->GetCanvasTop();
		}
		else if (!IsAutoSize(pChild->GetCanvasBottom()))
		{
			y = _finalSize.height - pChild->GetCanvasBottom() - desired.height;
		}

		pChild->Arrange(Rect(x, y, desired.width, desired.height));
	}
}

} // namespace sgui
