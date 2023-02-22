/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:29:35 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>
#include <SteinsGate/Common/Engine.h>

struct SGUIElementInfo : ConfigDataAbstract
{
	~SGUIElementInfo() override = default;

	UIElementType_t Type;
	SGString Name;
};

struct SGUIGroupElemInfo
{
	int Code;
	int ZOrder;
	float X;
	float Y;
};

struct SGUIGroupInfo : SGUIElementInfo
{
	SGUIGroupInfo(int elemCount) : InfoList(elemCount) {}
	~SGUIGroupInfo() override = default;

	SGRect Rect;
	HorizontalAlignment_t HorizontalAlignment;
	VerticalAlignment_t VerticalAlignment;
	SGVector<SGUIGroupElemInfo> InfoList;
};

struct SGUIButtonInfo : SGUIElementInfo
{
	~SGUIButtonInfo() override = default;

	int Sga;
	int Img;
	int Normal;
	int Over;
	int Pressed;
	int Disabled;
};

struct SGUILabelInfo : SGUIElementInfo
{
	~SGUILabelInfo() override = default;

	SGString Text;
	int FontCode;
	int FontSize;
	float Width;
	float Height;
	bool TextWrap;
	HorizontalAlignment_t TextHAlignment;
	VerticalAlignment_t TextVAlignment;
};

struct SGUISpriteInfo : SGUIElementInfo
{
	~SGUISpriteInfo() override = default;

	int Img;
	int Sga;
	int Sprite;
};


struct SGUIEditBoxInfo : SGUIElementInfo
{
	~SGUIEditBoxInfo() override = default;

	SGSize Size;
	int FontSize;
	SGColor4B FontColor;
	HorizontalAlignment_t TextHAlignment;
	SGString PlaceholderText;
	SGColor4B PlaceHolderFontColor;
	int PlaceholderFontSize;
	int MaxLength;
	SGInputMode InputMode;
};

struct SGUICheckBoxInfo : SGUIElementInfo
{
	int Sga;
	int Img;
	int Background;
	int BackgroundDisabled;
	int Cross;
	int CrossDisabled;
};