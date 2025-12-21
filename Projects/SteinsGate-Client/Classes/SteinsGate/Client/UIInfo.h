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
	UIElementType_t type_;
	HAlignment_t hAlignment_;
	VAlignment_t vAlignment_;
};

struct UIGroupElemInfo
{
	UIGroupElemInfo()
	{
		code_ = InvalidValue_v;
	}

	int code_;
	SGVec2 pos_;
};

struct UIGroupInfo : UIElementInfo
{
	UIGroupInfo(int _elemCount)
	: infoList_(_elemCount == 0 ? 1 : _elemCount)
	{
	} // 용량이 0일수는 없으므로.
	UIGroupInfo(const UIGroupInfo& _other) { this->operator=(_other); }
	UIGroupInfo(UIGroupInfo&& _other) noexcept { this->operator=(JCore::Move(_other)); }
	~UIGroupInfo() override = default;

	UIGroupInfo& operator=(const UIGroupInfo& _other);
	UIGroupInfo& operator=(UIGroupInfo&& _other) noexcept;

	SGSize size_;
	SGVector<UIGroupElemInfo> infoList_;
};

struct UIButtonInfo : UIElementInfo
{
	UIButtonInfo()
	{
		sga_ = InvalidValue_v;
		img_ = InvalidValue_v;
		SGArrays::Fill(sprites_, InvalidValue_v);
		linearDodge_ = false;
	}

	UIButtonInfo(const UIButtonInfo& _other) { this->operator=(_other); }
	UIButtonInfo& operator=(const UIButtonInfo& _other);

	int sga_;
	int img_;
	int sprites_[4];
	bool linearDodge_;
};

struct UILabelInfo : UIElementInfo
{
	UILabelInfo()
	{
		fontCode_ = 0;
		fontSize_ = 16;
		fontColor_ = { 0, 0, 0, 255 };
		text_ = "라벨 텍스트 없음";
		textWrap_ = true;
		textHAlignment_ = HAlignment::Left;
		textVAlignment_ = VAlignment::Top;
	}

	UILabelInfo(const UILabelInfo& _other) { this->operator=(_other); }
	UILabelInfo& operator=(const UILabelInfo& _other);

	SGSize size_;
	int fontCode_;
	int fontSize_;
	SGColor4B fontColor_;
	SGString text_;
	bool textWrap_;
	HAlignment_t textHAlignment_;
	VAlignment_t textVAlignment_;
};

struct UISpriteInfo : UIElementInfo
{
	UISpriteInfo()
	{
		img_ = InvalidValue_v;
		sga_ = InvalidValue_v;
		sprite_ = InvalidValue_v;
		linearDodge_ = false;
		scale9_ = false;
	}

	UISpriteInfo(const UISpriteInfo& _other) { this->operator=(_other); }
	UISpriteInfo& operator=(const UISpriteInfo& _other);

	int img_;
	int sga_;
	int sprite_;
	bool linearDodge_;
	bool scale9_;
	SGSize size_;
};


struct UIEditBoxInfo : UIElementInfo
{
	UIEditBoxInfo()
	{
		FontSize = 16;
		FontColor = { 0, 0, 0, 255 };
		TextHAlignment = HAlignment::Left;
		PlaceholderText = "텍스트 없음";
		PlaceHolderFontColor = { 128, 128, 128, 255 };
		PlaceholderFontSize = 16;
		MaxLength = 50;
		InputMode = SGInputMode::EMAIL_ADDRESS;
	}

	UIEditBoxInfo(const UIEditBoxInfo& _other) { this->operator=(_other); }
	UIEditBoxInfo& operator=(const UIEditBoxInfo& _other);

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

	UICheckBoxInfo(const UICheckBoxInfo& _other) { this->operator=(_other); }
	UICheckBoxInfo& operator=(const UICheckBoxInfo& _other);

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

	UIToggleButtonInfo(const UIToggleButtonInfo& _other) { this->operator=(_other); }
	UIToggleButtonInfo& operator=(const UIToggleButtonInfo& _other);

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

	UIScrollBarInfo(const UIScrollBarInfo& _other) { this->operator=(_other); }
	UIScrollBarInfo& operator=(const UIScrollBarInfo& _other);

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

	UIProgressBarInfo(const UIProgressBarInfo& _other) { this->operator=(_other); }
	UIProgressBarInfo& operator=(const UIProgressBarInfo& _other);

	int Sga;
	int Img;
	int Sprite;
	SGSize Size;
	ProgressIncreaseDirection_t ProgressIncreaseDirection;
};

struct UIStaticInfo : UIElementInfo
{
	UIStaticInfo() = default;
	UIStaticInfo(const UIStaticInfo& _other) { this->operator=(_other); }
	UIStaticInfo& operator=(const UIStaticInfo& _other);

	SGSize Size;
};
