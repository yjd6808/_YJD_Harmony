/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Xml/UIXmlLoader.h"
#include "sgcl/Game/UI/Xml/UIValue.h"
#include "sgcl/Game/UI/Xml/UIResolve.h"

#include "sgcl/Game/UI/Core/UIElement.h"
#include "sgcl/Game/UI/Core/Brush.h"
#include "sgcl/Game/UI/Panels/Panel.h"
#include "sgcl/Game/UI/Panels/Grid.h"
#include "sgcl/Game/UI/Panels/StackPanel.h"
#include "sgcl/Game/UI/Panels/Canvas.h"
#include "sgcl/Game/UI/Panels/DockPanel.h"
#include "sgcl/Game/UI/Controls/Control.h"
#include "sgcl/Game/UI/Controls/ContentControl.h"
#include "sgcl/Game/UI/Controls/TextBlock.h"
#include "sgcl/Game/UI/Controls/TextBox.h"
#include "sgcl/Game/UI/Controls/Button.h"
#include "sgcl/Game/UI/Controls/ToggleButton.h"
#include "sgcl/Game/UI/Controls/ProgressBar.h"
#include "sgcl/Game/UI/Controls/ScrollBar.h"
#include "sgcl/Game/UI/Controls/Slider.h"
#include "sgcl/Game/UI/Controls/Border.h"
#include "sgcl/Game/UI/Controls/Image.h"

#include <tinyxml2.h>

#include <cstdlib>
#include <cstring>
#include <vector>

USING_NS_CC;
USING_NS_JC;

namespace sgui
{

namespace
{

//////////////////////////////////////////////////////////////////////////////////////////
// 값 파싱 헬퍼
//////////////////////////////////////////////////////////////////////////////////////////
float ParseFloat(const UIValue& _value, float _default = 0.0f)
{
	return _value.IsNull() ? _default : (float)_value.ToFloat();
}

int ParseInt(const UIValue& _value, int _default = 0)
{
	return _value.IsNull() ? _default : (int)_value.ToInt();
}

bool ParseBool(const UIValue& _value, bool _default = false)
{
	return _value.IsNull() ? _default : _value.ToBool();
}

//////////////////////////////////////////////////////////////////////////////////////////
static bool IsDigitString(const std::string& _text)
{
	return !_text.empty() && _text.find_first_not_of("0123456789") == std::string::npos;
}

static std::vector<std::string> SplitTokens(const std::string& _text, const char* _delimiters)
{
	std::vector<std::string> tokens;
	size_t start = 0;

	while (start < _text.size())
	{
		while (start < _text.size() && strchr(_delimiters, _text[start]))
		{
			++start;
		}

		const size_t end = _text.find_first_of(_delimiters, start);
		if (end == std::string::npos)
		{
			if (start < _text.size())
			{
				tokens.push_back(_text.substr(start));
			}
			break;
		}

		if (end > start)
		{
			tokens.push_back(_text.substr(start, end - start));
		}

		start = end + 1;
	}

	return tokens;
}

//////////////////////////////////////////////////////////////////////////////////////////
static Thickness ParseThickness(const std::string& _text)
{
	const std::vector<std::string> tokens = SplitTokens(_text, ", \t");

	if (tokens.empty())
	{
		return Thickness();
	}

	if (tokens.size() == 1)
	{
		return Thickness((float)atof(tokens[0].c_str()));
	}

	if (tokens.size() == 2)
	{
		return Thickness((float)atof(tokens[0].c_str()), (float)atof(tokens[1].c_str()));
	}

	return Thickness(
		(float)atof(tokens[0].c_str()),
		(float)atof(tokens[1].c_str()),
		(float)atof(tokens[2].c_str()),
		tokens.size() >= 4 ? (float)atof(tokens[3].c_str()) : 0.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////
static bool TryParseHexByte(char _c, int& _out)
{
	if (_c >= '0' && _c <= '9') { _out = _c - '0'; return true; }
	if (_c >= 'a' && _c <= 'f') { _out = _c - 'a' + 10; return true; }
	if (_c >= 'A' && _c <= 'F') { _out = _c - 'A' + 10; return true; }
	return false;
}

static bool TryParseHexColor(const std::string& _text, UIColorF& _out)
{
	std::string hex = _text;

	if (!hex.empty() && hex[0] == '#')
	{
		hex = hex.substr(1);
	}

	if (hex.size() != 6 && hex.size() != 8)
	{
		return false;
	}

	const int count = (int)hex.size() / 2;
	uint32_t value = 0;

	for (int idx = 0; idx < count; ++idx)
	{
		int hi = 0;
		int lo = 0;
		if (!TryParseHexByte(hex[idx * 2], hi) || !TryParseHexByte(hex[idx * 2 + 1], lo))
		{
			return false;
		}
		value = (value << 8) | (uint32_t)((hi << 4) | lo);
	}

	if (hex.size() == 6)
	{
		value = 0xFF000000 | value;
	}

	_out = UIColorF::FromHex(value);
	return true;
}

static UIColorF ParseColor(const std::string& _text)
{
	UIColorF color;

	if (TryParseHexColor(_text, color))
	{
		return color;
	}

	if (_text == "Transparent")
	{
		return UIColorF(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (_text == "White") return UIColorF(1.0f, 1.0f, 1.0f, 1.0f);
	if (_text == "Black") return UIColorF(0.0f, 0.0f, 0.0f, 1.0f);
	if (_text == "Red") return UIColorF(1.0f, 0.0f, 0.0f, 1.0f);
	if (_text == "Green") return UIColorF(0.0f, 1.0f, 0.0f, 1.0f);
	if (_text == "Blue") return UIColorF(0.0f, 0.0f, 1.0f, 1.0f);

	// CSV/공백 r,g,b,a (0~255)
	const std::vector<std::string> tokens = SplitTokens(_text, ", \t");
	if (tokens.size() >= 3)
	{
		return UIColorF::FromRGBA(
			(uint8_t)atoi(tokens[0].c_str()),
			(uint8_t)atoi(tokens[1].c_str()),
			(uint8_t)atoi(tokens[2].c_str()),
			tokens.size() >= 4 ? (uint8_t)atoi(tokens[3].c_str()) : 255);
	}

	_LogWarn_("[UIXmlLoader] 색상 해석 실패: %s", _text.c_str());
	return UIColorF(1.0f, 1.0f, 1.0f, 1.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////
static HorizontalAlignment ParseHorizontalAlignment(const std::string& _text)
{
	if (_text == "Left") return HorizontalAlignment::Left;
	if (_text == "Center") return HorizontalAlignment::Center;
	if (_text == "Right") return HorizontalAlignment::Right;
	return HorizontalAlignment::Stretch;
}

static VerticalAlignment ParseVerticalAlignment(const std::string& _text)
{
	if (_text == "Top") return VerticalAlignment::Top;
	if (_text == "Center") return VerticalAlignment::Center;
	if (_text == "Bottom") return VerticalAlignment::Bottom;
	return VerticalAlignment::Stretch;
}

static Visibility ParseVisibility(const std::string& _text)
{
	if (_text == "Hidden") return Visibility::Hidden;
	if (_text == "Collapsed") return Visibility::Collapsed;
	return Visibility::Visible;
}

static WindowStyle ParseWindowStyle(const std::string& _text)
{
	if (_text == "Title" || _text == "TitleWindow") return WindowStyle::Title;
	return WindowStyle::None;
}

static Orientation ParseOrientation(const std::string& _text)
{
	return _text == "Horizontal" ? Orientation::Horizontal : Orientation::Vertical;
}

static Dock ParseDock(const std::string& _text)
{
	if (_text == "Top") return Dock::Top;
	if (_text == "Right") return Dock::Right;
	if (_text == "Bottom") return Dock::Bottom;
	return Dock::Left;
}

static Stretch ParseStretch(const std::string& _text)
{
	if (_text == "None") return Stretch::None;
	if (_text == "Uniform") return Stretch::Uniform;
	if (_text == "UniformToFill") return Stretch::UniformToFill;
	return Stretch::Fill;
}

static TextAlignment ParseTextAlignment(const std::string& _text)
{
	if (_text == "Center") return TextAlignment::Center;
	if (_text == "Right") return TextAlignment::Right;
	return TextAlignment::Left;
}

static TextWrapping ParseTextWrapping(const std::string& _text)
{
	return _text == "Wrap" ? TextWrapping::Wrap : TextWrapping::NoWrap;
}

//////////////////////////////////////////////////////////////////////////////////////////
static GridLength ParseGridLength(const std::string& _text)
{
	if (_text == "Auto" || _text == "auto")
	{
		return GridLength::Auto();
	}

	const std::string trimmed = _text;
	if (!trimmed.empty() && trimmed.back() == '*')
	{
		const std::string value = trimmed.substr(0, trimmed.size() - 1);
		const float star = value.empty() ? 1.0f : (float)atof(value.c_str());
		return GridLength::Star(star);
	}

	return GridLength::Px((float)atof(trimmed.c_str()));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 지연 바인딩 기록
//////////////////////////////////////////////////////////////////////////////////////////
struct PendingBinding
{
	UIElement* pElement;
	std::string name;
	std::string rawValue;
};

static bool HasBinding(const char* _value)
{
	return _value != nullptr && (strchr(_value, '{') != nullptr || strchr(_value, '`') != nullptr);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 요소 팩토리
//////////////////////////////////////////////////////////////////////////////////////////
UIElement* CreateElement(const char* _tag)
{
	if (strcmp(_tag, "Grid") == 0) return Grid::Create();
	if (strcmp(_tag, "StackPanel") == 0) return StackPanel::Create(Orientation::Vertical);
	if (strcmp(_tag, "Canvas") == 0) return Canvas::Create();
	if (strcmp(_tag, "DockPanel") == 0) return DockPanel::Create();
	if (strcmp(_tag, "Border") == 0) return Border::Create();
	if (strcmp(_tag, "TextBlock") == 0) return TextBlock::Create();
	if (strcmp(_tag, "Button") == 0) return Button::Create();
	if (strcmp(_tag, "CheckBox") == 0) return CheckBox::Create();
	if (strcmp(_tag, "ToggleButton") == 0) return ToggleButton::Create();
	if (strcmp(_tag, "RadioButton") == 0) return RadioButton::Create();
	if (strcmp(_tag, "ProgressBar") == 0) return ProgressBar::Create();
	if (strcmp(_tag, "ScrollBar") == 0) return ScrollBar::Create();
	if (strcmp(_tag, "Slider") == 0) return Slider::Create();
	if (strcmp(_tag, "TextBox") == 0) return TextBox::Create();
	if (strcmp(_tag, "Image") == 0) return Image::Create();
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 자식 부착
//////////////////////////////////////////////////////////////////////////////////////////
void AttachChild(UIElement* _pParent, UIElement* _pChild)
{
	// Window의 자식은 GetContentRoot()에 부착한다. (Title 스타일이면 콘텐츠 영역)
	if (Window* pWindow = dynamic_cast<Window*>(_pParent))
	{
		UIElement* pRoot = pWindow->GetContentRoot();

		if (pRoot != _pParent)
		{
			AttachChild(pRoot, _pChild);
			return;
		}
	}

	if (Panel* pPanel = dynamic_cast<Panel*>(_pParent))
	{
		pPanel->AddChild(_pChild);
		return;
	}

	if (Border* pBorder = dynamic_cast<Border*>(_pParent))
	{
		pBorder->SetChild(_pChild);
		return;
	}

	if (ContentControl* pContent = dynamic_cast<ContentControl*>(_pParent))
	{
		pContent->SetContent(_pChild);
		return;
	}

	_LogWarn_("[UIXmlLoader] %s 타입에는 자식을 부착할 수 없습니다.", _pParent->getName().c_str());
}

// Window 직속 자식이 아닌 위치에 ContentHost가 선언된 경우 오류 로그를 남긴다.
static void ValidateNoNestedContentHost(UIElement* _pElement, const char* _windowName)
{
	const int count = _pElement->GetChildElementCount();

	for (int idx = 0; idx < count; ++idx)
	{
		UIElement* pChild = _pElement->GetChildElementAt(idx);
		if (pChild == nullptr)
		{
			continue;
		}

		if (pChild->IsContentHost())
		{
			_LogError_("[UIXmlLoader] %s: ContentHost 속성은 Window 직속 자식에만 허용됩니다. (중첩 위치에서 발견: %s)",
				_windowName, pChild->getName().c_str());
		}

		ValidateNoNestedContentHost(pChild, _windowName);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// Grid 정의 컬렉션 (<Grid.ColumnDefinitions>/<Grid.RowDefinitions>)
//////////////////////////////////////////////////////////////////////////////////////////
void ApplyGridDefinitionCollection(Grid* _pGrid, tinyxml2::XMLElement* _pXml)
{
	const bool isColumn = strcmp(_pXml->Name(), "Grid.ColumnDefinitions") == 0;

	for (tinyxml2::XMLElement* pChild = _pXml->FirstChildElement(); pChild != nullptr; pChild = pChild->NextSiblingElement())
	{
		const char* pTag = pChild->Name();
		if (pTag == nullptr)
		{
			continue;
		}

		if (isColumn && strcmp(pTag, "ColumnDefinition") == 0)
		{
			ColumnDefinition definition;
			if (const char* pWidth = pChild->Attribute("Width"))
			{
				definition.width_ = ParseGridLength(pWidth);
			}
			if (const char* pMin = pChild->Attribute("MinWidth")) { definition.minWidth_ = (float)atof(pMin); }
			if (const char* pMax = pChild->Attribute("MaxWidth")) { definition.maxWidth_ = (float)atof(pMax); }
			_pGrid->AddColumnDefinition(definition);
		}
		else if (!isColumn && strcmp(pTag, "RowDefinition") == 0)
		{
			RowDefinition definition;
			if (const char* pHeight = pChild->Attribute("Height"))
			{
				definition.height_ = ParseGridLength(pHeight);
			}
			if (const char* pMin = pChild->Attribute("MinHeight")) { definition.minHeight_ = (float)atof(pMin); }
			if (const char* pMax = pChild->Attribute("MaxHeight")) { definition.maxHeight_ = (float)atof(pMax); }
			_pGrid->AddRowDefinition(definition);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 테마 색상 참조 브러시
// "theme:button.background"       → 상태 연동 (Normal/Hover/Pressed 자동 전환)
// "theme:button.hover.background" → 고정 상태 색상
//////////////////////////////////////////////////////////////////////////////////////////
bool TryParseThemeColorBrush(const std::string& _text, BrushPtr& _outBrush)
{
	if (_text.rfind("theme:", 0) != 0)
	{
		return false;
	}

	const std::vector<std::string> tokens = SplitTokens(_text.substr(6), ".");

	UIThemeControl control;
	UIThemeColorState state;
	UIThemeColorRole role;

	// "control.role" → 상태 연동 브러시
	if (tokens.size() == 2 && TryParseUIThemeControl(tokens[0].c_str(), control) && TryParseUIThemeColorRole(tokens[1].c_str(), role))
	{
		_outBrush = ThemeColorBrush::Create(control, role);
		return true;
	}

	// "control.state.role" → 고정 상태 브러시
	if (tokens.size() == 3 && TryParseUIThemeControl(tokens[0].c_str(), control) && TryParseUIThemeColorState(tokens[1].c_str(), state) && TryParseUIThemeColorRole(tokens[2].c_str(), role))
	{
		_outBrush = ThemeColorBrush::Create(MakeUIThemeColor(control, state, role));
		return true;
	}

	_LogWarn_("[UIXmlLoader] 테마 색상 참조 해석 실패: %s", _text.c_str());
	_outBrush = nullptr;
	return true;	// "theme:" 접두사는 소비하되 브러시는 설정하지 않는다.
}

//////////////////////////////////////////////////////////////////////////////////////////
// 배경 브러시 (색상 → SolidColorBrush, theme: 참조 → ThemeColorBrush)
//////////////////////////////////////////////////////////////////////////////////////////
BrushPtr CreateBackgroundBrush(const UIValue& _value)
{
	if (_value.IsNull())
	{
		return nullptr;
	}

	const std::string text = _value.ToString();

	BrushPtr pThemeBrush;
	if (TryParseThemeColorBrush(text, pThemeBrush))
	{
		return pThemeBrush;
	}

	return SolidColorBrush::Create(ParseColor(text));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 속성 적용
//////////////////////////////////////////////////////////////////////////////////////////
void ApplyAttribute(UIElement* _pElement, const char* _name, const UIValue& _value, const UIResolveContext& _ctx)
{
	UNUSED(_ctx);

	if (_pElement == nullptr || _name == nullptr)
	{
		return;
	}

	if (strcmp(_name, "Name") == 0)
	{
		_pElement->setName(_value.ToString().c_str());
		return;
	}

	// Window 전용 속성 (WPF WindowStyle 컨셉)
	if (Window* pWindow = dynamic_cast<Window*>(_pElement))
	{
		if (strcmp(_name, "WindowStyle") == 0) { pWindow->SetWindowStyle(ParseWindowStyle(_value.ToString())); return; }
		if (strcmp(_name, "Title") == 0) { pWindow->SetTitle(_value.ToString().c_str()); return; }
		if (strcmp(_name, "TitleBarHeight") == 0) { pWindow->SetTitleBarHeight(ParseFloat(_value)); return; }
		if (strcmp(_name, "TitleBarBackground") == 0) { pWindow->SetTitleBarBackground(ParseColor(_value.ToString())); return; }
		if (strcmp(_name, "TitleBarForeground") == 0) { pWindow->SetTitleBarForeground(ParseColor(_value.ToString())); return; }
		if (strcmp(_name, "WindowBackground") == 0) { pWindow->SetWindowBackground(ParseColor(_value.ToString())); return; }
		if (strcmp(_name, "WindowBorderBrush") == 0) { pWindow->SetWindowBorderBrush(ParseColor(_value.ToString())); return; }
	}

	if (strcmp(_name, "Width") == 0) { _pElement->SetWidth(ParseFloat(_value)); return; }
	if (strcmp(_name, "Height") == 0) { _pElement->SetHeight(ParseFloat(_value)); return; }
	if (strcmp(_name, "MinWidth") == 0) { _pElement->SetMinWidth(ParseFloat(_value)); return; }
	if (strcmp(_name, "MaxWidth") == 0) { _pElement->SetMaxWidth(ParseFloat(_value)); return; }
	if (strcmp(_name, "MinHeight") == 0) { _pElement->SetMinHeight(ParseFloat(_value)); return; }
	if (strcmp(_name, "MaxHeight") == 0) { _pElement->SetMaxHeight(ParseFloat(_value)); return; }
	if (strcmp(_name, "Margin") == 0) { _pElement->SetMargin(ParseThickness(_value.ToString())); return; }
	if (strcmp(_name, "HorizontalAlignment") == 0) { _pElement->SetHorizontalAlignment(ParseHorizontalAlignment(_value.ToString())); return; }
	if (strcmp(_name, "VerticalAlignment") == 0) { _pElement->SetVerticalAlignment(ParseVerticalAlignment(_value.ToString())); return; }
	if (strcmp(_name, "Visibility") == 0) { _pElement->SetVisibility(ParseVisibility(_value.ToString())); return; }
	if (strcmp(_name, "Visible") == 0) { _pElement->SetVisibility(ParseBool(_value) ? Visibility::Visible : Visibility::Collapsed); return; }
	if (strcmp(_name, "IsEnabled") == 0) { _pElement->SetIsEnabled(ParseBool(_value, true)); return; }
	if (strcmp(_name, "IsHitTestVisible") == 0) { _pElement->SetIsHitTestVisible(ParseBool(_value, true)); return; }
	if (strcmp(_name, "Foreground") == 0)
	{
		const std::string text = _value.ToString();
		BrushPtr pThemeBrush;
		if (TryParseThemeColorBrush(text, pThemeBrush)) { _pElement->SetForeground(pThemeBrush); return; }
		_pElement->SetForeground(ParseColor(text));
		return;
	}
	if (strcmp(_name, "FontSize") == 0) { _pElement->SetFontSize(ParseFloat(_value)); return; }
	if (strcmp(_name, "FontCode") == 0) { _pElement->SetFontCode(ParseInt(_value, -1)); return; }
	if (strcmp(_name, "Opacity") == 0) { _pElement->setOpacity((GLubyte)(jc::Math::Clamp(ParseFloat(_value, 1.0f), 0.0f, 1.0f) * 255.0f)); return; }

	// 부착 속성
	if (strcmp(_name, "Grid.Row") == 0) { _pElement->SetGridRow(ParseInt(_value)); return; }
	if (strcmp(_name, "Grid.Column") == 0) { _pElement->SetGridColumn(ParseInt(_value)); return; }
	if (strcmp(_name, "Grid.RowSpan") == 0) { _pElement->SetGridRowSpan(ParseInt(_value, 1)); return; }
	if (strcmp(_name, "Grid.ColumnSpan") == 0) { _pElement->SetGridColumnSpan(ParseInt(_value, 1)); return; }
	if (strcmp(_name, "Canvas.Left") == 0) { _pElement->SetCanvasLeft(ParseFloat(_value)); return; }
	if (strcmp(_name, "Canvas.Top") == 0) { _pElement->SetCanvasTop(ParseFloat(_value)); return; }
	if (strcmp(_name, "Canvas.Right") == 0) { _pElement->SetCanvasRight(ParseFloat(_value)); return; }
	if (strcmp(_name, "Canvas.Bottom") == 0) { _pElement->SetCanvasBottom(ParseFloat(_value)); return; }
	if (strcmp(_name, "Dock") == 0) { _pElement->SetDock(ParseDock(_value.ToString())); return; }
	if (strcmp(_name, "ContentHost") == 0) { _pElement->SetContentHost(ParseBool(_value)); return; }

	// 배경 (색상)
	if (strcmp(_name, "Background") == 0)
	{
		const std::string text = _value.ToString();

		// "Theme" 또는 "Theme:Semantic"이면 기본 테마를 따르도록 아무것도 설정하지 않는다.
		if (text.rfind("Theme", 0) == 0)
		{
			return;
		}

		BrushPtr pBrush = CreateBackgroundBrush(_value);
		if (Panel* pPanel = dynamic_cast<Panel*>(_pElement)) { pPanel->SetBackground(pBrush); return; }
		if (Control* pControl = dynamic_cast<Control*>(_pElement)) { pControl->SetBackground(pBrush); return; }
		return;
	}

	// 패딩
	if (strcmp(_name, "Padding") == 0)
	{
		const Thickness padding = ParseThickness(_value.ToString());
		if (Control* pControl = dynamic_cast<Control*>(_pElement)) { pControl->SetPadding(padding); return; }
		if (Border* pBorder = dynamic_cast<Border*>(_pElement)) { pBorder->SetPadding(padding); return; }
		return;
	}

	// ==================== 컨트롤 전용 ====================

	if (strcmp(_name, "Text") == 0)
	{
		if (TextBlock* pText = dynamic_cast<TextBlock*>(_pElement)) { pText->SetText(_value.ToString().c_str()); return; }
		if (TextBox* pBox = dynamic_cast<TextBox*>(_pElement)) { pBox->SetText(_value.ToString().c_str()); return; }
		return;
	}

	if (strcmp(_name, "TextAlignment") == 0)
	{
		if (TextBlock* pText = dynamic_cast<TextBlock*>(_pElement)) { pText->SetTextAlignment(ParseTextAlignment(_value.ToString())); return; }
		return;
	}

	if (strcmp(_name, "TextWrapping") == 0)
	{
		if (TextBlock* pText = dynamic_cast<TextBlock*>(_pElement)) { pText->SetTextWrapping(ParseTextWrapping(_value.ToString())); return; }
		return;
	}

	if (strcmp(_name, "HitTestEnabled") == 0)
	{
		if (TextBlock* pText = dynamic_cast<TextBlock*>(_pElement)) { pText->SetHitTestEnabled(ParseBool(_value)); return; }
		return;
	}

	if (strcmp(_name, "Content") == 0)
	{
		if (ContentControl* pContent = dynamic_cast<ContentControl*>(_pElement)) { pContent->SetContentText(_value.ToString().c_str()); return; }
		return;
	}

	if (strcmp(_name, "IsChecked") == 0)
	{
		if (ToggleButton* pToggle = dynamic_cast<ToggleButton*>(_pElement)) { pToggle->SetIsChecked(ParseBool(_value)); return; }
		return;
	}

	if (strcmp(_name, "GroupName") == 0)
	{
		if (RadioButton* pRadio = dynamic_cast<RadioButton*>(_pElement)) { pRadio->SetGroupName(_value.ToString().c_str()); return; }
		return;
	}

	if (strcmp(_name, "Percent") == 0)
	{
		if (ProgressBar* pBar = dynamic_cast<ProgressBar*>(_pElement)) { pBar->SetPercent(ParseFloat(_value)); return; }
		return;
	}

	if (strcmp(_name, "Minimum") == 0 || strcmp(_name, "Min") == 0)
	{
		if (RangeBase* pRange = dynamic_cast<RangeBase*>(_pElement)) { pRange->SetMinimum(ParseFloat(_value)); return; }
		return;
	}

	if (strcmp(_name, "Maximum") == 0 || strcmp(_name, "Max") == 0)
	{
		if (RangeBase* pRange = dynamic_cast<RangeBase*>(_pElement)) { pRange->SetMaximum(ParseFloat(_value)); return; }
		return;
	}

	if (strcmp(_name, "Value") == 0)
	{
		if (RangeBase* pRange = dynamic_cast<RangeBase*>(_pElement)) { pRange->SetValue(ParseFloat(_value)); return; }
		return;
	}

	if (strcmp(_name, "RowCount") == 0)
	{
		if (ScrollBar* pBar = dynamic_cast<ScrollBar*>(_pElement)) { pBar->SetRowCount(ParseInt(_value)); return; }
		return;
	}

	if (strcmp(_name, "RowCountPerPage") == 0)
	{
		if (ScrollBar* pBar = dynamic_cast<ScrollBar*>(_pElement)) { pBar->SetRowCountPerPage(ParseInt(_value, 1)); return; }
		return;
	}

	if (strcmp(_name, "RowPos") == 0)
	{
		if (ScrollBar* pBar = dynamic_cast<ScrollBar*>(_pElement)) { pBar->SetRowPos(ParseInt(_value)); return; }
		return;
	}

	if (strcmp(_name, "Placeholder") == 0)
	{
		if (TextBox* pBox = dynamic_cast<TextBox*>(_pElement)) { pBox->SetPlaceholder(_value.ToString().c_str()); return; }
		return;
	}

	if (strcmp(_name, "MaxLength") == 0)
	{
		if (TextBox* pBox = dynamic_cast<TextBox*>(_pElement)) { pBox->SetMaxLength(ParseInt(_value)); return; }
		return;
	}

	if (strcmp(_name, "PasswordMode") == 0)
	{
		if (TextBox* pBox = dynamic_cast<TextBox*>(_pElement)) { pBox->SetPasswordMode(ParseBool(_value)); return; }
		return;
	}

	if (strcmp(_name, "BorderBrush") == 0)
	{
		const std::string text = _value.ToString();

		BrushPtr pBrush;
		if (!TryParseThemeColorBrush(text, pBrush))
		{
			pBrush = SolidColorBrush::Create(ParseColor(text));
		}

		if (Border* pBorder = dynamic_cast<Border*>(_pElement)) { pBorder->SetBorderBrush(pBrush); return; }
		if (Control* pControl = dynamic_cast<Control*>(_pElement)) { pControl->SetBorderBrush(pBrush); return; }
		return;
	}

	if (strcmp(_name, "BorderThickness") == 0)
	{
		const Thickness thickness = ParseThickness(_value.ToString());
		if (Border* pBorder = dynamic_cast<Border*>(_pElement)) { pBorder->SetBorderThickness(thickness); return; }
		if (Control* pControl = dynamic_cast<Control*>(_pElement)) { pControl->SetBorderThickness(thickness); return; }
		return;
	}

	if (strcmp(_name, "Columns") == 0)
	{
		if (Grid* pGrid = dynamic_cast<Grid*>(_pElement))
		{
			for (const std::string& token : SplitTokens(_value.ToString(), ", \t"))
			{
				pGrid->AddColumnDefinition(ParseGridLength(token));
			}
			return;
		}
		return;
	}

	if (strcmp(_name, "Rows") == 0)
	{
		if (Grid* pGrid = dynamic_cast<Grid*>(_pElement))
		{
			for (const std::string& token : SplitTokens(_value.ToString(), ", \t"))
			{
				pGrid->AddRowDefinition(ParseGridLength(token));
			}
			return;
		}
		return;
	}

	if (strcmp(_name, "Orientation") == 0)
	{
		if (StackPanel* pStack = dynamic_cast<StackPanel*>(_pElement)) { pStack->SetOrientation(ParseOrientation(_value.ToString())); return; }
		return;
	}

	if (strcmp(_name, "Spacing") == 0)
	{
		if (StackPanel* pStack = dynamic_cast<StackPanel*>(_pElement)) { pStack->SetSpacing(ParseFloat(_value)); return; }
		return;
	}

	if (strcmp(_name, "LastChildFill") == 0)
	{
		if (DockPanel* pDock = dynamic_cast<DockPanel*>(_pElement)) { pDock->SetLastChildFill(ParseBool(_value, true)); return; }
		return;
	}

	if (strcmp(_name, "Source") == 0)
	{
		if (Image* pImage = dynamic_cast<Image*>(_pElement)) { pImage->SetSource(_value.ToString().c_str()); return; }
		return;
	}

	if (strcmp(_name, "Stretch") == 0)
	{
		if (Image* pImage = dynamic_cast<Image*>(_pElement)) { pImage->SetStretch(ParseStretch(_value.ToString())); return; }
		return;
	}

	_LogWarn_("[UIXmlLoader] 알 수 없는 속성: %s (요소: %s)", _name, _pElement->getName().c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////
// 요소 트리 빌드 (1패스: 정적 속성 + 이름 설정, 바인딩은 지연 수집)
//////////////////////////////////////////////////////////////////////////////////////////
UIElement* BuildElement(
	tinyxml2::XMLElement* _pXml,
	const UIResolveContext& _ctx,
	std::vector<PendingBinding>& _pending)
{
	const char* pTag = _pXml->Name();
	UIElement* pElement = CreateElement(pTag);

	if (pElement == nullptr)
	{
		_LogWarn_("[UIXmlLoader] 알 수 없는 태그: %s", pTag ? pTag : "null");
		return nullptr;
	}

	// 이름 우선 설정 (로그 등에 사용)
	if (const char* pName = _pXml->Attribute("Name"))
	{
		pElement->setName(pName);
	}

	for (const tinyxml2::XMLAttribute* pAttr = _pXml->FirstAttribute(); pAttr != nullptr; pAttr = pAttr->Next())
	{
		const char* pAttrName = pAttr->Name();
		const char* pValue = pAttr->Value();

		if (pAttrName == nullptr || pValue == nullptr)
		{
			continue;
		}

		if (strcmp(pAttrName, "Name") == 0)
		{
			continue;	// 이미 설정
		}

		if (HasBinding(pValue))
		{
			_pending.push_back({ pElement, pAttrName, pValue });
			continue;
		}

		ApplyAttribute(pElement, pAttrName, UIValue::MakeString(pValue), _ctx);
	}

	// 자식 요소 (DataList/Data는 데이터이므로 제외)
	for (tinyxml2::XMLElement* pChild = _pXml->FirstChildElement(); pChild != nullptr; pChild = pChild->NextSiblingElement())
	{
		const char* pChildTag = pChild->Name();
		if (pChildTag == nullptr || strcmp(pChildTag, "DataList") == 0 || strcmp(pChildTag, "Data") == 0)
		{
			continue;
		}

		// Grid 정의 컬렉션은 트리에 부착하지 않고 Grid에 등록한다.
		if ((strcmp(pChildTag, "Grid.ColumnDefinitions") == 0 || strcmp(pChildTag, "Grid.RowDefinitions") == 0)
			&& dynamic_cast<Grid*>(pElement))
		{
			ApplyGridDefinitionCollection(static_cast<Grid*>(pElement), pChild);
			continue;
		}

		UIElement* pChildElement = BuildElement(pChild, _ctx, _pending);
		if (pChildElement)
		{
			AttachChild(pElement, pChildElement);
		}
	}

	return pElement;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 윈도우 생성
//////////////////////////////////////////////////////////////////////////////////////////
// 문서를 파싱해 기존 Window 인스턴스에 레이아웃 트리를 병합한다. (루트 <Window>)
static bool PopulateWindowFromDocument(Window* pWindow, tinyxml2::XMLDocument* _pDoc)
{
	tinyxml2::XMLElement* pRoot = _pDoc->RootElement();
	if (pRoot == nullptr)
	{
		_LogError_("[UIXmlLoader] 루트 요소가 없습니다.");
		return false;
	}

	if (strcmp(pRoot->Name(), "Window") != 0)
	{
		_LogError_("[UIXmlLoader] 루트 태그는 <Window>여야 합니다. (현재: %s)", pRoot->Name());
		return false;
	}

	if (const char* pName = pRoot->Attribute("Name"))
	{
		pWindow->setName(pName);
	}

	// 전역 데이터 (DataList)
	UIDataList windowData;
	if (tinyxml2::XMLElement* pDataList = pRoot->FirstChildElement("DataList"))
	{
		windowData = ParseDataList(pDataList);
	}

	UIResolveContext ctx;
	ctx.pRoot = pWindow;
	ctx.pData = &windowData;

	std::vector<PendingBinding> pending;

	for (const tinyxml2::XMLAttribute* pAttr = pRoot->FirstAttribute(); pAttr != nullptr; pAttr = pAttr->Next())
	{
		const char* pAttrName = pAttr->Name();
		const char* pValue = pAttr->Value();

		if (pAttrName == nullptr || pValue == nullptr || strcmp(pAttrName, "Name") == 0)
		{
			continue;
		}

		if (HasBinding(pValue))
		{
			pending.push_back({ pWindow, pAttrName, pValue });
			continue;
		}

		ApplyAttribute(pWindow, pAttrName, UIValue::MakeString(pValue), ctx);
	}

	std::vector<UIElement*> pendingChildren;

	for (tinyxml2::XMLElement* pChild = pRoot->FirstChildElement(); pChild != nullptr; pChild = pChild->NextSiblingElement())
	{
		const char* pChildTag = pChild->Name();
		if (pChildTag == nullptr || strcmp(pChildTag, "DataList") == 0 || strcmp(pChildTag, "Data") == 0)
		{
			continue;
		}

		UIElement* pChildElement = BuildElement(pChild, ctx, pending);
		if (pChildElement)
		{
			pendingChildren.push_back(pChildElement);
		}
	}

	// ContentHost 검증: Window 직속 자식 중 정확히 1개만 ContentHost="true"여야 한다.
	UIElement* pHost = nullptr;

	for (UIElement* pChild : pendingChildren)
	{
		if (!pChild->IsContentHost())
		{
			continue;
		}

		if (pHost)
		{
			_LogError_("[UIXmlLoader] %s: ContentHost 속성은 Window 직속 자식에 1개만 허용됩니다. (2개 이상 선언됨)",
				pWindow->getName().c_str());
			pHost = nullptr;
		}
		else
		{
			pHost = pChild;
		}
	}

	if (pHost == nullptr)
	{
		_LogError_("[UIXmlLoader] %s: ContentHost 패널이 선언되지 않았습니다. Window 직속 자식에 ContentHost=\"true\" 패널을 선언하세요.",
			pWindow->getName().c_str());
	}
	else
	{
		pWindow->SetContentHost(pHost);
	}

	// 호스트를 제외한 나머지 자식을 콘텐츠 루트에 부착한다.
	for (UIElement* pChild : pendingChildren)
	{
		if (pChild == pHost)
		{
			continue;
		}

		AttachChild(pWindow, pChild);
	}

	// 중첩 위치에 ContentHost가 선언된 경우도 오류로 처리한다.
	for (UIElement* pChild : pendingChildren)
	{
		if (pChild != pHost)
		{
			ValidateNoNestedContentHost(pChild, pWindow->getName().c_str());
		}
	}

	// 2패스: 바인딩 속성 지연 적용 (트리 완성 후 해석)
	for (const PendingBinding& binding : pending)
	{
		UIResolveContext applyCtx = ctx;
		applyCtx.pCurrent = binding.pElement;
		const UIValue resolved = ResolveAttribute(binding.rawValue, applyCtx);
		ApplyAttribute(binding.pElement, binding.name.c_str(), resolved, ctx);
	}

	return true;
}

Window* LoadWindowFromDocument(tinyxml2::XMLDocument* _pDoc)
{
	Window* pWindow = dbg_new Window;
	pWindow->autorelease();

	if (!PopulateWindowFromDocument(pWindow, _pDoc))
	{
		return nullptr;
	}

	pWindow->SetXmlLoaded(true);
	return pWindow;
}

} // namespace

//////////////////////////////////////////////////////////////////////////////////////////
Window* UIXmlLoader::LoadWindowFromFile(const char* _filePath)
{
	if (_filePath == nullptr)
	{
		return nullptr;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(_filePath) != tinyxml2::XML_SUCCESS)
	{
		_LogError_("[UIXmlLoader] XML 파일 로드 실패: %s", _filePath);
		return nullptr;
	}

	return LoadWindowFromDocument(&doc);
}

//////////////////////////////////////////////////////////////////////////////////////////
Window* UIXmlLoader::LoadWindowFromString(const char* _xmlContent)
{
	if (_xmlContent == nullptr)
	{
		return nullptr;
	}

	tinyxml2::XMLDocument doc;
	if (doc.Parse(_xmlContent) != tinyxml2::XML_SUCCESS)
	{
		_LogError_("[UIXmlLoader] XML 문자열 파싱 실패");
		return nullptr;
	}

	return LoadWindowFromDocument(&doc);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIXmlLoader::LoadWindowInto(Window* _pWindow, const char* _filePath)
{
	if (_pWindow == nullptr || _filePath == nullptr)
	{
		return false;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(_filePath) != tinyxml2::XML_SUCCESS)
	{
		_LogError_("[UIXmlLoader] XML 파일 로드 실패: %s", _filePath);
		return false;
	}

	if (!PopulateWindowFromDocument(_pWindow, &doc))
	{
		return false;
	}

	_pWindow->SetXmlLoaded(true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIXmlLoader::LoadWindowIntoString(Window* _pWindow, const char* _xmlContent)
{
	if (_pWindow == nullptr || _xmlContent == nullptr)
	{
		return false;
	}

	tinyxml2::XMLDocument doc;
	if (doc.Parse(_xmlContent) != tinyxml2::XML_SUCCESS)
	{
		_LogError_("[UIXmlLoader] XML 문자열 파싱 실패");
		return false;
	}

	if (!PopulateWindowFromDocument(_pWindow, &doc))
	{
		return false;
	}

	_pWindow->SetXmlLoaded(true);
	return true;
}

} // namespace sgui
