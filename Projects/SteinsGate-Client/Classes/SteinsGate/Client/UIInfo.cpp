/*
 * 작성자: 윤정도
 * 생성일: 8/19/2023 7:58:09 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "UIInfo.h"

USING_NS_JC;

UIGroupInfo& UIGroupInfo::operator=(const UIGroupInfo& other) {
	Size = other.Size;
	InfoList = other.InfoList;
	return *this;
}

UIGroupInfo& UIGroupInfo::operator=(UIGroupInfo&& other) noexcept {
	Size = other.Size;
	InfoList = Move(other.InfoList);
	return *this;
}

UIButtonInfo& UIButtonInfo::operator=(const UIButtonInfo& other) {
	Sga = other.Sga;
	Img = other.Img;
	LinearDodge = other.LinearDodge;
	Arrays::Copy(Sprites, other.Sprites);
	return *this;
}

UILabelInfo& UILabelInfo::operator=(const UILabelInfo& other) {
	Size = other.Size;
	FontCode = other.FontCode;
	FontSize = other.FontSize;
	FontColor = other.FontColor;
	Text = other.Text;
	TextWrap = other.TextWrap;
	TextHAlignment = other.TextHAlignment;
	TextVAlignment = other.TextVAlignment;
	return *this;
}

UISpriteInfo& UISpriteInfo::operator=(const UISpriteInfo& other) {
	Img = other.Img;
	Sga = other.Sga;
	Sprite = other.Sprite;
	LinearDodge = other.LinearDodge;
	Scale9 = other.Scale9;
	Size = other.Size;
	return *this;
}

UIEditBoxInfo& UIEditBoxInfo::operator=(const UIEditBoxInfo& other) {
	Size = other.Size;
	FontSize = other.FontSize;
	FontColor = other.FontColor;
	TextHAlignment = other.TextHAlignment;
	PlaceholderText = other.PlaceholderText;
	PlaceHolderFontColor = other.PlaceHolderFontColor;
	PlaceholderFontSize = other.PlaceholderFontSize;
	MaxLength = other.MaxLength;
	InputMode = other.InputMode;
	return *this;
}

UICheckBoxInfo& UICheckBoxInfo::operator=(const UICheckBoxInfo& other) {
	Check = other.Check;
	BackgroundSga = other.BackgroundSga;
	BackgroundImg = other.BackgroundImg;
	CrossSga = other.CrossSga;
	CrossImg = other.CrossImg;
	Arrays::Copy(Sprites, other.Sprites);
	return *this;
}

UIToggleButtonInfo& UIToggleButtonInfo::operator=(const UIToggleButtonInfo& other) {
	Sga = other.Sga;
	Img = other.Img;
	Arrays::Copy(Sprites[0], other.Sprites[0]);
	Arrays::Copy(Sprites[1], other.Sprites[1]);
	LinearDodge = other.LinearDodge;
	return *this;
}

UIScrollBarInfo& UIScrollBarInfo::operator=(const UIScrollBarInfo& other) {
	Sga = other.Sga;
	Img = other.Img;
	Arrays::Copy(Sprites, other.Sprites);
	TrackSize = other.TrackSize;
	return *this;
}

UIProgressBarInfo& UIProgressBarInfo::operator=(const UIProgressBarInfo& other) {
	Sga = other.Sga;
	Img = other.Img;
	Sprite = other.Sprite;
	Size = other.Size;
	ProgressIncreaseDirection = other.ProgressIncreaseDirection;
	return *this;
}

UIStaticInfo& UIStaticInfo::operator=(const UIStaticInfo& other) {
	Size = other.Size;
	return *this;
}

