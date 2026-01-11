/*
 * 작성자: 윤정도
 * 생성일: 8/19/2023 1:20:08 PM
 * =====================
 *
 */


#pragma once

#include <sg/_Struct/SteinsGate_ChannelBase.h>
#include <sgcl/UIRootGroup.h>

#define SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT	10

struct ChannelInfo;
class UI_ChannelSelect : public UIRootGroup
{
public:
	UI_ChannelSelect(UIGroupInfo* _pGroupInfo);
	~UI_ChannelSelect() override;

	enum class ChannelTab
	{
		Teen, // 청소년
		Adult // 성인
	};

	struct ServerButton
	{
		enum State
		{
			sSelected,
			sNormal,
			sDisabled
		};

		ServerButton(GameServerType_t _type);

		void InitSprites(GameServerType_t _type);
		void SetState(State _state);
		void Select();
		void Normal();
		void Disabled();

		bool ContainsPoint(const cc::vec2& _pos);

		GameServerType_t type_;
		UIGroup* pGroup_;
		UISprite* pSpriteSelectedBackground_;
		UISprite* pSpriteSelectedText_;
		UISprite* pSpriteSelectedBorder_;
		UISprite* pSpriteNormalText_;
		UISprite* pSpriteNormalBorder_;
		UISprite* pSpriteDisabledText_;
		UISprite* pSpriteServerCharacter_;
		State state_;
	};

	struct ChannelButton
	{
		enum State
		{
			sNormal,
			sSelected,
			sInvisible,
		};

		ChannelButton(int _index);

		void InitSprites();

		void SetState(State _state);
		void SetNormal();
		void SetSelected();
		void SetInvisible();

		void OnMouseOver();
		void OnMouseLeave();

		void InitMonsterSprites(UI_ChannelSelect* _pRootGroup);
		void SetChannelInfo(const LobbyChannelInfo& _channelInfo, char _sequence);

		bool ContainsPoint(const cc::vec2& _pos);

		int index_;
		UIToggleButton* pToggleBtnEnteranceBackground_[EnteranceType::Max];
		UISprite* pSpriteMouseOverBorder_;
		UISprite* pSpriteMonsters_[2]; /* 직접생성, index 0: selected, index 1: normal */
		UILabel* pLabelName_;
		UILabel* pLabelDensity_;
		State state_;
		ChannelInfo* pInfo_;
	};

protected:
	void OnInit() override;
	void OnLoaded() override;
	void OnUnloaded() override;
	void OnUpdate(float _dt) override;

	void OnMouseDown(cc::EventMouse* _pMouseEvent) override;
	void OnMouseMove(cc::EventMouse* _pMouseEvent) override;
	void OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state) override;

	int GetMonsterSpriteSgaIndex() const { return monsterSpriteSgaIndex_; }
	int GetMonsterSpriteImgIndex() const { return monsterSpriteImgIndex_; }

	void UpdateServerSelectionSpriteOpacity(float _dt);
	void UpdateBackgroundGearRotation(float _dt);

public:
	// void initServerList();
	// void initChannelList();

	void SelectServer(GameServerType_t _serverType);
	void SelectChannel(int _channelIndex);
	void SelectChannelTab(ChannelTab _tab);

	void EnterChannel(GameServerType_t _serverType, int _channelIndex);

private:
	UISprite* pSpriteBackgroundGear_;
	UIButton* pBtnStart_;
	UIButton* pBtnTerminate_;
	UIButton* pBtnRefresh_;
	UIToggleButton* pToggleTeen_;
	UIToggleButton* pToggleBtnAdult_;
	UILabel* pLabelPage_;

	ServerButton* pServerButtons_[GameServerType::Max];
	ChannelButton* pChannelButtons_[SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT];

	// =========================================

	int monsterSpriteSgaIndex_;
	int monsterSpriteImgIndex_;

	float serverButtonSelectionOpacity_;
	bool serverButtonSelectionOpacityUp_;

	GameServerType_t selectedServer_;
	int selectedChannelIndex_;
	ChannelTab channelTab_;
};
