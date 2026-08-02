/*
 * 작성자: 윤정도
 * =====================
 * sgui - 최상위 UI 화면 단위 (WPF Window)
 * 각 게임 화면(UI_Login 등)은 Window를 상속받아 구현하고 REGISTER_WINDOW로 등록한다.
 * 생명주기: OnInit -> (레이아웃) -> OnLoaded -> OnAdded -> OnUpdate... -> OnRemoved -> OnClosed
 */

#pragma once

#include "sgcl/Game/UI/Panels/Canvas.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "jc/Primitives/String.h"

namespace sgui
{

class UIHost;
class Border;
class Grid;
class TextBlock;
class StackPanel;
class Button;
class BrushVisual;

// WPF WindowStyle: 화면 창의 프레임 스타일
enum class WindowStyle : uint8_t
{
	None,	// 기본: 투명 배경, 전체 화면 (기존 Window 동작)
	Title	// 타이틀바 + min/max/close 캡션 버튼
};

class Window : public Canvas
{
	friend class UIHost;
	friend class UIManager;

public:
	Window();
	virtual ~Window() override;

	// ==================== 생명주기 ====================
	virtual void OnInit(const jc::CDataMap<>& _dataMap) { UNUSED(_dataMap); }
	virtual void OnLoaded() {}
	virtual void OnAdded() {}
	virtual void OnRemoved() {}
	virtual void OnClosed() {}
	virtual void OnUpdate(float _delta) { UNUSED(_delta); }

	// 호스트에서 포커스된 윈도우에 전달되는 키 이벤트. true 반환 시 소비.
	virtual bool OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode) { UNUSED(_keyCode); return false; }
	virtual bool OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode) { UNUSED(_keyCode); return false; }

	virtual void OnEventRaised(int _id, UIEvent* _pEvent) override;

	// ==================== 윈도우 제어 ====================
	void Close();	// 프레임 말미에 지연 제거된다.
	bool IsCloseRequested() const { return closeRequested_; }

	void SetModal(bool _modal) { isModal_ = _modal; }
	bool IsModal() const { return isModal_; }

	void SetCloseWithEscape(bool _enable) { closeWithEscape_ = _enable; }
	bool IsCloseWithEscape() const { return closeWithEscape_; }

	void SetDragMoveEnabled(bool _enable) { dragMoveEnabled_ = _enable; }
	bool IsDragMoveEnabled() const { return dragMoveEnabled_; }

	void SetWindowPosition(float _x, float _y);
	void CenterOnHost();
	void MoveWindowBy(float _deltaX, float _deltaY);

	// ==================== 스타일 (WPF WindowStyle) ====================
	void SetWindowStyle(WindowStyle _style);
	WindowStyle GetWindowStyle() const { return windowStyle_; }

	void SetTitle(const char* _title);
	const char* GetTitle() const { return title_.Source(); }

	// 최소화/최대화 (Title 스타일 전용)
	void Minimize();
	void Maximize();
	void Restore();
	void ToggleMaximize();
	bool IsMinimized() const { return state_ == WindowState::Minimized; }
	bool IsMaximized() const { return state_ == WindowState::Maximized; }

	// Title 스타일 구성 (기본값: 현재 스킴 기반 다크/라이트 톤, 명시 설정 시 우선)
	void SetTitleBarHeight(float _height);
	float GetTitleBarHeight() const { return titleBarHeight_; }
	void SetTitleBarBackground(const UIColorF& _color);
	void SetTitleBarForeground(const UIColorF& _color);
	void SetWindowBackground(const UIColorF& _color);
	void SetWindowBorderBrush(const UIColorF& _color);

	// XML/코드에서 자식 요소를 부착하는 대상.
	// ContentHost 패널이 선언된 경우: 호스트 / 미선언(오류 폴백): this
	virtual UIElement* GetContentRoot()
	{
		return pContentHost_ ? pContentHost_ : this;
	}

	// XML ContentHost 패널을 등록한다. (UIXmlLoader 전용)
	// Title: 레이아웃 그리드 1행(콘텐츠 영역) / None: 창 전체 영역
	void SetContentHost(UIElement* _pHost);

	virtual void RefreshThemeVisuals() override;

	// ==================== 유틸 ====================
	UIHost* GetHost() const { return pHost_; }

	// 이름으로 하위 요소 검색 (WPF FindName)
	UIElement* FindName(const char* _name);

	template <typename T>
	T* FindName(const char* _name) { return dynamic_cast<T*>(FindName(_name)); }

	// ==================== XML 로드 상태 ====================
	// UIXmlLoader가 레이아웃 XML을 이 윈도우에 병합하면 true가 된다.
	// OnInit()에서 이 플래그를 확인해 XML 기반 초기화(FindElementByName) 또는 코드 빌드 폴백을 구분한다.
	void SetXmlLoaded(bool _loaded) { bXmlLoaded_ = _loaded; }
	bool IsXmlLoaded() const { return bXmlLoaded_; }

	void MarkLayoutDirty() { layoutDirty_ = true; }
	bool IsLayoutDirty() const { return layoutDirty_; }
	void FlushLayout(const cc::size& _hostSize);

protected:
	virtual void OnRenderSizeChanged(const cc::size& _size) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;

	UIHost* pHost_ = nullptr;
	bool isModal_ = false;
	bool closeRequested_ = false;
	bool closeWithEscape_ = false;
	bool dragMoveEnabled_ = false;
	bool layoutDirty_ = true;
	bool useExplicitPosition_ = false;
	cc::vec2 explicitPosition_;	// y-down 호스트 좌표
	bool dragMoveActive_ = false;
	cc::vec2 dragMoveLast_;			// 월드(y-up) 좌표
	bool bXmlLoaded_ = false;		// 레이아웃 XML 병합 여부

	// ==================== Title 스타일 상태 ====================
	enum class WindowState : uint8_t
	{
		Normal,
		Minimized,
		Maximized
	};

	WindowStyle windowStyle_ = WindowStyle::None;
	WindowState state_ = WindowState::Normal;
	jc::String title_ = "Untitled";
	float titleBarHeight_ = 32.0f;

	UIColorF titleBarBackground_ { 0.16f, 0.17f, 0.20f, 1.0f };
	UIColorF titleBarForeground_ { 0.92f, 0.93f, 0.95f, 1.0f };
	UIColorF windowBackground_ { 0.10f, 0.11f, 0.13f, 1.0f };
	UIColorF windowBorderBrush_ { 0.30f, 0.31f, 0.34f, 1.0f };

	bool customTitleBarBackground_ = false;
	bool customWindowBackground_ = false;
	bool customWindowBorderBrush_ = false;

	// 최소화/최대화 전 정상 상태 위치+크기 (y-down 호스트 좌표)
	bool hasSavedRect_ = false;
	float savedX_ = 0.0f;
	float savedY_ = 0.0f;
	float savedWidth_ = 0.0f;
	float savedHeight_ = 0.0f;

	// ==================== Title 크롬 요소 ====================
	void BuildTitleBarChrome();
	void DestroyTitleBarChrome();
	Button* BuildCaptionButton(UIAssetSemantic _semantic, const char* _name, const char* _fallbackText);
	void ApplyChromeColors();
	void SaveNormalRect();
	void SyncChromeSize();

	Border* pChrome_ = nullptr;
	Grid* pLayoutGrid_ = nullptr;
	Border* pTitleBar_ = nullptr;
	Grid* pTitleBarGrid_ = nullptr;
	TextBlock* pTitleLabel_ = nullptr;
	StackPanel* pCaptionButtonStack_ = nullptr;
	Button* pBtnMinimize_ = nullptr;
	Button* pBtnMaximize_ = nullptr;
	Button* pBtnClose_ = nullptr;
	UIElement* pContentHost_ = nullptr;

	jc::Vector<BrushVisual*> iconVisuals_;
};

} // namespace sgui
