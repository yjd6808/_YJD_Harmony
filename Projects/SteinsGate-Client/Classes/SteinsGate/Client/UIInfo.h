/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:29:35 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>
#include <SteinsGate/Common/Engine.h>

struct UIElementInfo : ConfigDataAbstract
{
	UIElementType_t Type;
	HAlignment_t HAlignment;
	VAlignment_t VAlignment;
};

struct UIGroupElemInfo
{
	UIGroupElemInfo()
	{
		Code = InvalidValue_v;
	}

	int Code;
	SGVec2 Pos;
};

struct UIGroupInfo : UIElementInfo
{
	UIGroupInfo(int elemCount) : InfoList(elemCount) {}
	~UIGroupInfo() override = default;

	SGSize Size;
	SGVector<UIGroupElemInfo> InfoList;
};

struct UIButtonInfo : UIElementInfo
{
	UIButtonInfo()
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

struct UILabelInfo : UIElementInfo
{
	UILabelInfo()
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

struct UISpriteInfo : UIElementInfo
{
	UISpriteInfo()
	{
		Img = InvalidValue_v;
		Sga = InvalidValue_v;
		Sprite = InvalidValue_v;
		LinearDodge = false;
		Scale9 = false;
	}

	int Img;
	int Sga;
	int Sprite;
	bool LinearDodge;
	bool Scale9;
	SGSize Size;
};


struct UIEditBoxInfo : UIElementInfo
{
	UIEditBoxInfo()
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

struct UICheckBoxInfo : UIElementInfo
{
	UICheckBoxInfo()
	{
		Check = false;
		BackgroundSga = InvalidValue_v;
		BackgroundImg = InvalidValue_v;
		CrossSga = InvalidValue_v;
		CrossImg = InvalidValue_v;
		SGArrays::Fill(Sprites, InvalidValue_v);
	}

	bool Check;
	int BackgroundSga;
	int BackgroundImg;
	int CrossSga;
	int CrossImg;
	int Sprites[4];
};

struct UIToggleButtonInfo : UIElementInfo
{
	UIToggleButtonInfo()
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

struct UIScrollBarInfo : UIElementInfo
{
	UIScrollBarInfo()
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

struct UIProgressBarInfo : UIElementInfo
{
	UIProgressBarInfo()
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

struct UIStaticInfo : UIElementInfo
{
	SGSize Size;
};

