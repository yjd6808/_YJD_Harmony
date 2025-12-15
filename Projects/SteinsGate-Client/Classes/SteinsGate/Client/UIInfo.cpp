/*
 * 작성자: 윤정도
 * 생성일: 8/19/2023 7:58:09 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "UIInfo.h"

USING_NS_JC;

UIGroupInfo& UIGroupInfo::operator=(const UIGroupInfo& _other)
{
	Size = _other.Size;
	InfoList = _other.InfoList;
	return *this;
}

UIGroupInfo& UIGroupInfo::operator=(UIGroupInfo&& _other) noexcept
{
	Size = _other.Size;
	InfoList = Move(_other.InfoList);
	return *this;
}

UIButtonInfo& UIButtonInfo::operator=(const UIButtonInfo& _other)
{
	Sga = _other.Sga;
	Img = _other.Img;
	LinearDodge = _other.LinearDodge;
	Arrays::Copy(Sprites, _other.Sprites);
	return *this;
}

UILabelInfo& UILabelInfo::operator=(const UILabelInfo& _other)
{
	Size = _other.Size;
	FontCode = _other.FontCode;
	FontSize = _other.FontSize;
	FontColor = _other.FontColor;
	Text = _other.Text;
	TextWrap = _other.TextWrap;
	TextHAlignment = _other.TextHAlignment;
	TextVAlignment = _other.TextVAlignment;
	return *this;
}

UISpriteInfo& UISpriteInfo::operator=(const UISpriteInfo& _other)
{
	Img = _other.Img;
	Sga = _other.Sga;
	Sprite = _other.Sprite;
	LinearDodge = _other.LinearDodge;
	Scale9 = _other.Scale9;
	Size = _other.Size;
	return *this;
}

UIEditBoxInfo& UIEditBoxInfo::operator=(const UIEditBoxInfo& _other)
{
	Size = _other.Size;
	FontSize = _other.FontSize;
	FontColor = _other.FontColor;
	TextHAlignment = _other.TextHAlignment;
	PlaceholderText = _other.PlaceholderText;
	PlaceHolderFontColor = _other.PlaceHolderFontColor;
	PlaceholderFontSize = _other.PlaceholderFontSize;
	MaxLength = _other.MaxLength;
	InputMode = _other.InputMode;
	return *this;
}

UICheckBoxInfo& UICheckBoxInfo::operator=(const UICheckBoxInfo& _other)
{
	Check = _other.Check;
	BackgroundSga = _other.BackgroundSga;
	BackgroundImg = _other.BackgroundImg;
	CrossSga = _other.CrossSga;
	CrossImg = _other.CrossImg;
	Arrays::Copy(Sprites, _other.Sprites);
	return *this;
}

UIToggleButtonInfo& UIToggleButtonInfo::operator=(const UIToggleButtonInfo& _other)
{
	Sga = _other.Sga;
	Img = _other.Img;
	Arrays::Copy(Sprites[0], _other.Sprites[0]);
	Arrays::Copy(Sprites[1], _other.Sprites[1]);
	LinearDodge = _other.LinearDodge;
	return *this;
}

UIScrollBarInfo& UIScrollBarInfo::operator=(const UIScrollBarInfo& _other)
{
	Sga = _other.Sga;
	Img = _other.Img;
	Arrays::Copy(Sprites, _other.Sprites);
	TrackSize = _other.TrackSize;
	return *this;
}

UIProgressBarInfo& UIProgressBarInfo::operator=(const UIProgressBarInfo& _other)
{
	Sga = _other.Sga;
	Img = _other.Img;
	Sprite = _other.Sprite;
	Size = _other.Size;
	ProgressIncreaseDirection = _other.ProgressIncreaseDirection;
	return *this;
}

UIStaticInfo& UIStaticInfo::operator=(const UIStaticInfo& _other)
{
	Size = _other.Size;
	return *this;
}
