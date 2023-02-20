/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:29:35 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct SGUIElementInfo : ConfigDataAbstract
{
	~SGUIElementInfo() override = default;

	UIElementType_t Type;
	SGString Name;
};

struct SGUIButtonInfo : SGUIElementInfo
{
	~SGUIButtonInfo() override = default;

	int Img;
	int Sga;
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
	HorizontalAlignment_t HorizontalAlignment;
	VerticalAlignment_t VerticalAlignment;
};

struct SGUISpriteInfo : SGUIElementInfo
{
	~SGUISpriteInfo() override = default;

	int Img;
	int Sga;
	int Sprite;
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