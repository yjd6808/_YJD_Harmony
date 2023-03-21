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
	UIElementType_t Type;
	HAlignment_t HAlignment;
	VAlignment_t VAlignment;
};

struct SGUIGroupElemInfo
{
	SGUIGroupElemInfo()
	{
		Code = InvalidValue_v;
	}

	int Code;
	SGVec2 Pos;
};

struct SGUIGroupInfo : SGUIElementInfo
{
	SGUIGroupInfo(int elemCount) : InfoList(elemCount) {}
	~SGUIGroupInfo() override = default;

	SGSize Size;
	SGVector<SGUIGroupElemInfo> InfoList;
};

struct SGUIButtonInfo : SGUIElementInfo
{
	SGUIButtonInfo()
	{
		Sga = InvalidValue_v;
		Img = InvalidValue_v;
		SGArrays::Fill(Sprites, InvalidValue_v);
		LinearDodge = false;
	}

	int Sga;
	int Img;
	int Sprites[4];
	bool LinearDodge;
};

struct SGUILabelInfo : SGUIElementInfo
{
	SGUILabelInfo()
	{
		FontCode = 0;
		FontSize = 16;
		FontColor = { 0, 0, 0, 255 };
		Text = "라벨 텍스트 없음";
		TextWrap = true;
		TextHAlignment = HAlignment::Left;
		TextVAlignment = VAlignment::Top;
	}

	SGSize Size;
	int FontCode;
	int FontSize;
	SGColor4B FontColor;
	SGString Text;
	bool TextWrap;
	HAlignment_t TextHAlignment;
	VAlignment_t TextVAlignment;
};

struct SGUISpriteInfo : SGUIElementInfo
{
	SGUISpriteInfo()
	{
		Img = InvalidValue_v;
		Sga = InvalidValue_v;
		Sprite = InvalidValue_v;
		LinearDodge = false;

	}

	int Img;
	int Sga;
	int Sprite;
	bool LinearDodge;
	SGSize Size;
};


struct SGUIEditBoxInfo : SGUIElementInfo
{
	SGUIEditBoxInfo()
	{
		FontSize = 16;
		FontColor = {0, 0, 0, 255};
		TextHAlignment = HAlignment::Left;
		PlaceholderText = "텍스트 없음";
		PlaceHolderFontColor = { 128, 128, 128, 255 };
		PlaceholderFontSize = 16;
		MaxLength = 50;
		InputMode = SGInputMode::EMAIL_ADDRESS;
	}
	

	SGSize Size;
	int FontSize;
	SGColor4B FontColor;
	HAlignment_t TextHAlignment;
	SGString PlaceholderText;
	SGColor4B PlaceHolderFontColor;
	int PlaceholderFontSize;
	int MaxLength;
	SGInputMode InputMode;
};

struct SGUICheckBoxInfo : SGUIElementInfo
{
	SGUICheckBoxInfo()
	{
		BackgroundSga = InvalidValue_v;
		BackgroundImg = InvalidValue_v;
		CrossSga = InvalidValue_v;
		CrossImg = InvalidValue_v;
		SGArrays::Fill(Sprites, InvalidValue_v);
	}

	int BackgroundSga;
	int BackgroundImg;
	int CrossSga;
	int CrossImg;
	int Sprites[4];
};

struct SGUIToggleButtonInfo : SGUIElementInfo
{
	SGUIToggleButtonInfo()
	{
		Sga = InvalidValue_v;
		Img = InvalidValue_v;
		SGArrays::Fill(Sprites[0], InvalidValue_v);
		SGArrays::Fill(Sprites[1], InvalidValue_v);
		LinearDodge = false;
	}

	int Sga;
	int Img;
	int Sprites[2][4];
	bool LinearDodge;
};

struct SGUIScrollBarInfo : SGUIElementInfo
{
	SGUIScrollBarInfo()
	{
		Sga = InvalidValue_v;
		Img = InvalidValue_v;
		SGArrays::Fill(Sprites, InvalidValue_v);
	}

	static constexpr int IndexUpNormal = 0;
	static constexpr int IndexUpOver = 1;
	static constexpr int IndexDownNormal = 2;
	static constexpr int IndexDownOver = 3;
	static constexpr int IndexThumbNormal = 4;
	static constexpr int IndexThumbOver = 5;
	static constexpr int IndexTrack = 6;

	int Sga;
	int Img;
	int Sprites[7];
	SGSize TrackSize;
};

struct SGUIProgressBarInfo : SGUIElementInfo
{
	SGUIProgressBarInfo()
	{
		Sga = InvalidValue_v;
		Img = InvalidValue_v;
		Sprite = InvalidValue_v;
		ProgressIncreaseDirection = ProgressIncreaseDirection::LeftRight;
	}


	int Sga;
	int Img;
	int Sprite;
	SGSize Size;
	ProgressIncreaseDirection_t ProgressIncreaseDirection;
};

struct SGUIStaticInfo : SGUIElementInfo
{
	SGSize Size;
};

