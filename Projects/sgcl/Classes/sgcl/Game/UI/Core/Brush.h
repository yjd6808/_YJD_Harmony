/*
 * 작성자: 윤정도
 * =====================
 * sgui - WPF 스타일 Brush 계층
 * 모든 텍스처/색상 표현은 Brush로 통일한다. (테마 기반 렌더링 전용)
 * - SolidColorBrush / LinearGradientBrush : 단순 색상 렌더링
 * - ThemeBrush : UIThemeManager가 베이크한 테마 텍스처(UIAssetSemantic)
 */

#pragma once

#include "sgcl/Game/UI/Core/UITypes.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIThemeColor.h"

#include <memory>

namespace sgui
{

enum class BrushType : uint8_t
{
	SolidColor,
	LinearGradient,
	Theme,
	ThemeColor
};

class Brush
{
public:
	virtual ~Brush() = default;
	virtual BrushType GetType() const = 0;
};

using BrushPtr = std::shared_ptr<Brush>;

class SolidColorBrush : public Brush
{
public:
	explicit SolidColorBrush(const UIColorF& _color) : color_(_color) {}
	virtual BrushType GetType() const override { return BrushType::SolidColor; }

	static BrushPtr Create(const UIColorF& _color) { return std::make_shared<SolidColorBrush>(_color); }
	static BrushPtr Create(float _r, float _g, float _b, float _a = 1.0f) { return Create(UIColorF{ _r, _g, _b, _a }); }

	UIColorF color_;
};

class LinearGradientBrush : public Brush
{
public:
	LinearGradientBrush(const UIColorF& _start, const UIColorF& _end, bool _vertical) : start_(_start), end_(_end), vertical_(_vertical) {}
	virtual BrushType GetType() const override { return BrushType::LinearGradient; }

	static BrushPtr Create(const UIColorF& _start, const UIColorF& _end, bool _vertical = true) { return std::make_shared<LinearGradientBrush>(_start, _end, _vertical); }

	UIColorF start_;
	UIColorF end_;
	bool vertical_;
};

// 테마 베이커가 생성한 텍스처를 참조하는 브러시.
// controlType_ + 현재 VisualState로 스타일을 Resolve하고, semantic_으로 텔스처를 찾는다.
class ThemeBrush : public Brush
{
public:
	ThemeBrush(UIAssetSemantic _semantic, UIElementType_t _controlType, UIComponentSlot _slot, bool _fixedSize)
	: semantic_(_semantic), controlType_(_controlType), slot_(_slot), fixedSize_(_fixedSize) {}

	virtual BrushType GetType() const override { return BrushType::Theme; }

	static BrushPtr Create(
		UIAssetSemantic _semantic,
		UIElementType_t _controlType,
		UIComponentSlot _slot = UIComponentSlot::Background,
		bool _fixedSize = false)
	{
		return std::make_shared<ThemeBrush>(_semantic, _controlType, _slot, _fixedSize);
	}

	UIAssetSemantic semantic_;
	UIElementType_t controlType_;
	UIComponentSlot slot_;
	bool fixedSize_;	// true면 Scale9 대신 원본 크기 스프라이트로 바인딩 (노브/마크/도트 등)
};

// 테마 색상 테이블(UIThemeColorTable)을 참조하는 단색 브러시. (WPF DynamicResource 컨셉)
// followsState_가 true면 BrushVisual의 VisualState에 따라 상태별 색상을 자동 전환하고,
// false면 fixedState_의 색상을 고정 사용한다.
class ThemeColorBrush : public Brush
{
public:
	ThemeColorBrush(UIThemeControl _control, UIThemeColorRole _role, bool _followsState, UIThemeColorState _fixedState)
	: control_(_control), role_(_role), followsState_(_followsState), fixedState_(_fixedState) {}

	virtual BrushType GetType() const override { return BrushType::ThemeColor; }

	// 상태 연동 브러시 (예: 버튼 배경 - Normal/Hover/Pressed 색상 자동 전환)
	static BrushPtr Create(UIThemeControl _control, UIThemeColorRole _role)
	{
		return std::make_shared<ThemeColorBrush>(_control, _role, true, UIThemeColorState::Normal);
	}

	// 고정 상태 브러시 (예: UIThemeColor::ButtonHoverBackground)
	static BrushPtr Create(UIThemeColor _color)
	{
		const UIThemeColorParts parts = DecomposeUIThemeColor(_color);
		return std::make_shared<ThemeColorBrush>(parts.control, parts.role, false, parts.state);
	}

	UIThemeControl control_;
	UIThemeColorRole role_;
	bool followsState_;
	UIThemeColorState fixedState_;
};

// 브러시를 현재 테마 기준 단색으로 해석한다. (구현: BrushVisual.cpp)
// SolidColor=색상 그대로, LinearGradient=시작색, ThemeColor=테마 색상 테이블, Theme=Resolve된 표면 상단색
UIColorF ResolveBrushColor(const Brush* _pBrush, UIVisualState _state);
inline UIColorF ResolveBrushColor(const BrushPtr& _brush, UIVisualState _state) { return ResolveBrushColor(_brush.get(), _state); }

inline cc::Color4B ToColor4B(const UIColorF& _color)
{
	return cc::Color4B(
		(GLubyte)(_color.r * 255.0f),
		(GLubyte)(_color.g * 255.0f),
		(GLubyte)(_color.b * 255.0f),
		(GLubyte)(_color.a * 255.0f));
}

inline cc::Color3B ToColor3B(const UIColorF& _color)
{
	return cc::Color3B(
		(GLubyte)(_color.r * 255.0f),
		(GLubyte)(_color.g * 255.0f),
		(GLubyte)(_color.b * 255.0f));
}

} // namespace sgui
