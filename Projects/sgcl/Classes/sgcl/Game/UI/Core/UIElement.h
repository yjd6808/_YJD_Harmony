/*
 * 작성자: 윤정도
 * =====================
 * sgui - 모든 UI 요소의 베이스 (WPF UIElement/FrameworkElement 역할)
 * - Measure/Arrange 2패스 레이아웃 (좌상단 원점 y-down 좌표계, 코코스 변환은 CommitToCocos)
 * - Margin / Width / Height / Min / Max / Alignment / Visibility
 * - Foreground / FontCode / FontSize 상속 속성
 * - 라우티드 이벤트: RaiseEvent() -> 터널링(Preview) -> 버블링, OnEventRaised(int _id, UIEvent*) 단일 수신부
 * - 생성 시 ElementRegistry에서 고유 ID를 중복 없이 할당받는다
 */

#pragma once

#include "sgcl/Game/UI/Core/UITypes.h"
#include "sgcl/Game/UI/Core/UIEvent.h"
#include "sgcl/Game/UI/Core/Brush.h"

#include <functional>
#include <vector>

namespace sgui
{

class Window;

class UIElement : public cc::Node
{
public:
	using EventHandler = std::function<void(UIElement*, UIEvent*)>;

	UIElement();
	virtual ~UIElement() override;

	// ==================== 식별 ====================
	_u32 GetElementId() const { return elementId_; }

	template <typename TElement>
	TElement* Cast() { return static_cast<TElement*>(this); }

	// ==================== 논리 트리 ====================
	UIElement* GetParentElement() const { return pParentElement_; }
	void SetParentElement(UIElement* _pParent) { pParentElement_ = _pParent; }

	virtual int GetChildElementCount() const { return 0; }
	virtual UIElement* GetChildElementAt(int _index) const { UNUSED(_index); return nullptr; }

	Window* GetWindow() const;

	UIElement* FindElementByName(const char* _name);

	template <typename TElement>
	TElement* FindElementByName(const char* _name) { return dynamic_cast<TElement*>(FindElementByName(_name)); }

	void ForEachRecursiveContainedSelf(const std::function<void(UIElement*)>& _fn);

	// ==================== 레이아웃 속성 ====================
	void SetMargin(const Thickness& _margin) { margin_ = _margin; InvalidateLayout(); }
	const Thickness& GetMargin() const { return margin_; }

	void SetHorizontalAlignment(HorizontalAlignment _align) { hAlignment_ = _align; InvalidateLayout(); }
	HorizontalAlignment GetHorizontalAlignment() const { return hAlignment_; }

	void SetVerticalAlignment(VerticalAlignment _align) { vAlignment_ = _align; InvalidateLayout(); }
	VerticalAlignment GetVerticalAlignment() const { return vAlignment_; }

	void SetWidth(float _width)
	{
		if (width_ == _width)
		{
			return;
		}
		width_ = _width;
		InvalidateLayout();
	}
	float GetWidth() const { return width_; }

	void SetHeight(float _height)
	{
		if (height_ == _height)
		{
			return;
		}
		height_ = _height;
		InvalidateLayout();
	}
	float GetHeight() const { return height_; }

	void SetSize(float _width, float _height)
	{
		if (width_ == _width && height_ == _height)
		{
			return;
		}
		width_ = _width;
		height_ = _height;
		InvalidateLayout();
	}

	void SetMinWidth(float _v) { minWidth_ = _v; InvalidateLayout(); }
	void SetMaxWidth(float _v) { maxWidth_ = _v; InvalidateLayout(); }
	void SetMinHeight(float _v) { minHeight_ = _v; InvalidateLayout(); }
	void SetMaxHeight(float _v) { maxHeight_ = _v; InvalidateLayout(); }

	void SetVisibility(Visibility _visibility);
	Visibility GetVisibility() const { return visibility_; }

	void SetIsEnabled(bool _enabled);
	bool IsEnabled() const { return isEnabled_; }
	bool IsEffectivelyEnabled() const;

	void SetIsHitTestVisible(bool _visible) { isHitTestVisible_ = _visible; }
	bool IsHitTestVisible() const { return isHitTestVisible_; }

	// ==================== 부착(attached) 속성 ====================
	void SetGridRow(int _row) { gridRow_ = _row; InvalidateLayout(); }
	int GetGridRow() const { return gridRow_; }
	void SetGridColumn(int _column) { gridColumn_ = _column; InvalidateLayout(); }
	int GetGridColumn() const { return gridColumn_; }
	void SetGridRowSpan(int _span) { gridRowSpan_ = _span > 1 ? _span : 1; InvalidateLayout(); }
	int GetGridRowSpan() const { return gridRowSpan_; }
	void SetGridColumnSpan(int _span) { gridColumnSpan_ = _span > 1 ? _span : 1; InvalidateLayout(); }
	int GetGridColumnSpan() const { return gridColumnSpan_; }

	void SetCanvasLeft(float _v) { canvasLeft_ = _v; InvalidateLayout(); }
	float GetCanvasLeft() const { return canvasLeft_; }
	void SetCanvasTop(float _v) { canvasTop_ = _v; InvalidateLayout(); }
	float GetCanvasTop() const { return canvasTop_; }
	void SetCanvasRight(float _v) { canvasRight_ = _v; InvalidateLayout(); }
	float GetCanvasRight() const { return canvasRight_; }
	void SetCanvasBottom(float _v) { canvasBottom_ = _v; InvalidateLayout(); }
	float GetCanvasBottom() const { return canvasBottom_; }

	void SetDock(Dock _dock) { dock_ = _dock; InvalidateLayout(); }
	Dock GetDock() const { return dock_; }

	// Window 직속 자식 중 콘텐츠 호스트로 선언된 요소 (UIXmlLoader 전용)
	void SetContentHost(bool _v) { isContentHost_ = _v; }
	bool IsContentHost() const { return isContentHost_; }

	// ==================== 상속 속성 (Foreground / Font) ====================
	// WPF처럼 Foreground는 Brush로 저장한다. (UIColorF 오버로드는 SolidColorBrush로 래핑)
	void SetForeground(const UIColorF& _color) { SetForeground(SolidColorBrush::Create(_color)); }
	void SetForeground(const BrushPtr& _brush) { hasForeground_ = _brush != nullptr; foregroundBrush_ = _brush; OnInheritedPropertyChanged(); }
	void ClearForeground() { hasForeground_ = false; foregroundBrush_ = nullptr; OnInheritedPropertyChanged(); }
	const BrushPtr& GetForegroundBrush() const { return foregroundBrush_; }
	UIColorF GetEffectiveForeground() const;

	void SetFontCode(int _fontCode) { fontCode_ = _fontCode; OnInheritedPropertyChanged(); }
	int GetEffectiveFontCode() const;

	void SetFontSize(float _fontSize) { fontSize_ = _fontSize; OnInheritedPropertyChanged(); }
	float GetEffectiveFontSize() const;

	// ==================== 레이아웃 ====================
	void Measure(const cc::size& _available);
	void Arrange(const Rect& _finalRect);

	const cc::size& GetDesiredSize() const { return desiredSize_; }
	const cc::size& GetRenderSize() const { return renderSize_; }
	const Rect& GetLayoutRect() const { return layoutRect_; }

	void InvalidateLayout();

	// ==================== 이벤트 ====================
	int AddHandler(UIEventType _type, const EventHandler& _handler, bool _handledEventsToo = false);
	void RemoveHandler(UIEventType _type, int _token);

	// 터널링(Preview) -> 버블링 순서로 라우팅한다. (Direct 전략은 대상에만 전달)
	void RaiseEvent(UIEvent* _pEvent);

	// 단일 이벤트 수신부. _id는 이벤트를 최초 발생시킨 요소의 고유 ID.
	virtual void OnEventRaised(int _id, UIEvent* _pEvent) { UNUSED(_id); UNUSED(_pEvent); }

	void CaptureMouse();
	void ReleaseMouseCapture();
	bool IsMouseCaptured() const;

	virtual void Focus();
	bool IsFocused() const;

	// ==================== 히트테스트 ====================
	virtual UIElement* HitTest(const cc::vec2& _worldPos);
	virtual bool IsSelfHitTestVisible() const { return isHitTestVisible_; }
	bool ContainsWorldPoint(const cc::vec2& _worldPos) const;
	cc::vec2 WorldToLocal(const cc::vec2& _worldPos) const;

	// ==================== 상태 / 테마 ====================
	bool IsMouseOver() const { return isMouseOver_; }
	virtual void UpdateVisualState() {}
	virtual void RefreshThemeVisuals();

	// 전경색(Foreground) 관련 비주얼만 갱신한다. (레이아웃 무효화 없음)
	// Control::UpdateVisualState()가 상태 변화 시 자식 텍스트 색을 다시 동기화할 때 사용.
	virtual void RefreshForegroundVisuals();

	virtual jc::String ToString();

protected:
	virtual cc::size MeasureOverride(const cc::size& _constraint) { UNUSED(_constraint); return cc::size(0.0f, 0.0f); }
	virtual void ArrangeOverride(const cc::size& _finalSize) { UNUSED(_finalSize); }
	virtual void OnRenderSizeChanged(const cc::size& _size) { UNUSED(_size); }
	virtual void OnInheritedPropertyChanged();

	// 자신의 유효 전경색을 제공하면 true를 반환한다. (Control은 테마 색상 테이블로 폴백)
	virtual bool TryGetForegroundColor(UIColorF& _outColor) const;

	void CommitToCocos();
	void DeliverEvent(UIEvent* _pEvent);

	static float ClampF(float _value, float _min, float _max);

	friend class InputDispatcher;

	struct HandlerEntry
	{
		int token_;
		UIEventType type_;
		bool handledEventsToo_;
		EventHandler handler_;
	};

	_u32 elementId_;
	UIElement* pParentElement_ = nullptr;

	Thickness margin_;
	HorizontalAlignment hAlignment_ = HorizontalAlignment::Stretch;
	VerticalAlignment vAlignment_ = VerticalAlignment::Stretch;
	float width_ = AutoSize_v;
	float height_ = AutoSize_v;
	float minWidth_ = 0.0f;
	float maxWidth_ = Infinity_v;
	float minHeight_ = 0.0f;
	float maxHeight_ = Infinity_v;
	Visibility visibility_ = Visibility::Visible;
	bool isEnabled_ = true;
	bool isHitTestVisible_ = true;
	bool isMouseOver_ = false;

	int gridRow_ = 0;
	int gridColumn_ = 0;
	int gridRowSpan_ = 1;
	int gridColumnSpan_ = 1;
	float canvasLeft_ = AutoSize_v;
	float canvasTop_ = AutoSize_v;
	float canvasRight_ = AutoSize_v;
	float canvasBottom_ = AutoSize_v;
	Dock dock_ = Dock::Left;
	bool isContentHost_ = false;

	bool hasForeground_ = false;
	BrushPtr foregroundBrush_;
	int fontCode_ = -1;			// -1: 부모로부터 상속
	float fontSize_ = -1.0f;	// 음수: 부모로부터 상속

	cc::size desiredSize_;
	cc::size renderSize_;
	Rect layoutRect_;

	std::vector<HandlerEntry> handlers_;
	int nextHandlerToken_ = 1;
};

} // namespace sgui
