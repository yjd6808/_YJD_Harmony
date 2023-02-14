/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:29:35 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

struct SGUIElementInfo
{
	UIElementType_t Type;
	int Code;
	SGString Name;
	int ZOrder;
};

struct SGUIButtonInfo : SGUIElementInfo
{
	int Img;
	int Npk;
	int Normal;
	int Over;
	int Pressed;
	int Disabled;
};

struct SGUILabelInfo : SGUIElementInfo
{
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
	int Img;
	int Npk;
	int Sprite;
};


struct SGUIGroupElemInfo
{
	SGUIElementInfo* ElemInfo;
	float X;
	float Y;
};

struct SGUIGroupInfo
{
	SGUIGroupInfo(int elemCount) : InfoList(elemCount) {}

	int Code;
	SGString Name;
	SGRect Rect;
	HorizontalAlignment_t HorizontalAlignment;
	VerticalAlignment_t VerticalAlignment;
	SGVector<SGUIGroupElemInfo> InfoList;
};