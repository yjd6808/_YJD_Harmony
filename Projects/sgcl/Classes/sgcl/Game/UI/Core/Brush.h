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

#include <memory>

namespace sgui
{

enum class BrushType : uint8_t
{
	SolidColor,
	LinearGradient,
	Theme
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
