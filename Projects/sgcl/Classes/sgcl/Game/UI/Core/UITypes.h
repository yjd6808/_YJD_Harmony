/*
 * 작성자: 윤정도
 * =====================
 * sgui - WPF 스타일 UI 프레임워크 공용 타입
 */

#pragma once

#include "sg/Struct/SteinsGate_UI.h"

#include <cmath>
#include <limits>

namespace sgui
{

constexpr float AutoSize_v = std::numeric_limits<float>::quiet_NaN();
constexpr float Infinity_v = std::numeric_limits<float>::infinity();

inline bool IsAutoSize(float _value) { return std::isnan(_value); }

enum class Visibility : uint8_t
{
	Visible,
	Hidden,		// 공간은 차지하지만 그리지 않음
	Collapsed	// 공간도 차지하지 않음
};

enum class HorizontalAlignment : uint8_t { Left, Center, Right, Stretch };
enum class VerticalAlignment : uint8_t { Top, Center, Bottom, Stretch };
enum class Orientation : uint8_t { Horizontal, Vertical };
enum class Dock : uint8_t { Left, Top, Right, Bottom };
enum class Stretch : uint8_t { None, Fill, Uniform, UniformToFill };
enum class TextAlignment : uint8_t { Left, Center, Right };
enum class TextWrapping : uint8_t { NoWrap, Wrap };

struct Thickness
{
	float left_ = 0.0f;
	float top_ = 0.0f;
	float right_ = 0.0f;
	float bottom_ = 0.0f;

	Thickness() = default;
	explicit Thickness(float _uniform) : left_(_uniform), top_(_uniform), right_(_uniform), bottom_(_uniform) {}
	Thickness(float _horizontal, float _vertical) : left_(_horizontal), top_(_vertical), right_(_horizontal), bottom_(_vertical) {}
	Thickness(float _left, float _top, float _right, float _bottom) : left_(_left), top_(_top), right_(_right), bottom_(_bottom) {}

	float Width() const { return left_ + right_; }
	float Height() const { return top_ + bottom_; }
};

struct CornerRadius
{
	float topLeft_ = 0.0f;
	float topRight_ = 0.0f;
	float bottomRight_ = 0.0f;
	float bottomLeft_ = 0.0f;

	CornerRadius() = default;
	explicit CornerRadius(float _uniform) : topLeft_(_uniform), topRight_(_uniform), bottomRight_(_uniform), bottomLeft_(_uniform) {}
};

// sgui 레이아웃 좌표계: 좌상단 원점, y는 아래로 증가 (WPF와 동일)
// 코코스 노드 좌표계(y 위로 증가) 변환은 UIElement::CommitToCocos()에서 일괄 처리한다.
struct Rect
{
	float x_ = 0.0f;
	float y_ = 0.0f;
	float width_ = 0.0f;
	float height_ = 0.0f;

	Rect() = default;
	Rect(float _x, float _y, float _width, float _height) : x_(_x), y_(_y), width_(_width), height_(_height) {}

	float Right() const { return x_ + width_; }
	float Bottom() const { return y_ + height_; }
	bool Contains(float _x, float _y) const { return _x >= x_ && _x < Right() && _y >= y_ && _y < Bottom(); }
};

enum class GridUnitType : uint8_t { Auto, Pixel, Star };

struct GridLength
{
	float value_ = 1.0f;
	GridUnitType type_ = GridUnitType::Star;

	static GridLength Auto() { return { 0.0f, GridUnitType::Auto }; }
	static GridLength Px(float _value) { return { _value, GridUnitType::Pixel }; }
	static GridLength Star(float _value = 1.0f) { return { _value, GridUnitType::Star }; }
};

struct RowDefinition
{
	GridLength height_ = GridLength::Star();
	float minHeight_ = 0.0f;
	float maxHeight_ = Infinity_v;
};

struct ColumnDefinition
{
	GridLength width_ = GridLength::Star();
	float minWidth_ = 0.0f;
	float maxWidth_ = Infinity_v;
};

} // namespace sgui
