/*
 * 작성자: 윤정도
 * 생성일: 8/19/2023 7:58:09 PM
 * =====================
 *
 */


#include "Core.h"
#include "SteinsGate_UI.h"

USING_NS_JC;

UIGroupInfo& UIGroupInfo::operator=(const UIGroupInfo& _other)
{
	size_ = _other.size_;
	infoList_ = _other.infoList_;
	return *this;
}

UIGroupInfo& UIGroupInfo::operator=(UIGroupInfo&& _other) noexcept
{
	size_ = _other.size_;
	infoList_ = Move(_other.infoList_);
	return *this;
}

UIButtonInfo& UIButtonInfo::operator=(const UIButtonInfo& _other)
{
	sga_ = _other.sga_;
	img_ = _other.img_;
	linearDodge_ = _other.linearDodge_;
	Arrays::Copy(sprites_, _other.sprites_);
	return *this;
}

UILabelInfo& UILabelInfo::operator=(const UILabelInfo& _other)
{
	size_ = _other.size_;
	fontCode_ = _other.fontCode_;
	fontSize_ = _other.fontSize_;
	fontColor_ = _other.fontColor_;
	text_ = _other.text_;
	textWrap_ = _other.textWrap_;
	textHAlignment_ = _other.textHAlignment_;
	textVAlignment_ = _other.textVAlignment_;
	return *this;
}

UISpriteInfo& UISpriteInfo::operator=(const UISpriteInfo& _other)
{
	img_ = _other.img_;
	sga_ = _other.sga_;
	sprite_ = _other.sprite_;
	linearDodge_ = _other.linearDodge_;
	scale9_ = _other.scale9_;
	size_ = _other.size_;
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
