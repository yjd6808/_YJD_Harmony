/*
 * 작성자: 윤정도
 * 생성일: 8/19/2023 1:20:08 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIRootGroup.h>

#define SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT	10

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
			Selected,
			Normal,
			Disabled
		};

		ServerButton(GameServerType_t Type);

		void initSprites(GameServerType_t Type);
		void setState(State state);
		void select();
		void normal();
		void disabled();

		bool containsPoint(const SGVec2& pos);

		GameServerType_t Type;
		UIGroup* Group;
		UISprite* SpriteSelectedBackground;
		UISprite* SpriteSelectedText;
		UISprite* SpriteSelectedBorder;
		UISprite* SpriteNormalText;
		UISprite* SpriteNormalBorder;
		UISprite* SpriteDisabledText;
		UISprite* SpriteServerCharacter;
		State State_;
	};

	struct ChannelButton
	{
		enum State
		{
			Normal,
			Selected,
			Invisible,
		};

		ChannelButton(int index);

		void initSprites();

		void setState(State state);
		void normal();
		void selected();
		void invisible();
		void mouseOver();
		void mouseLeave();

		void initMonsterSprites(UI_ChannelSelect* masterGroup);
		void setChannelInfo(const LobbyChannelInfo& channelInfo, char sequence);

		bool containsPoint(const SGVec2& pos);

		int Index;
		UIToggleButton* TBtnEnteranceBackground[EnteranceType::Max];
		UISprite* SpriteMouseOverBorder;
		UISprite* SpriteMonster[2]; /* 직접생성, index 0: selected, index 1: normal */
		UILabel* LabelName;
		UILabel* LabelDensity;
		State State_;
		ChannelInfo* Info;
	};

protected:
	void OnInit() override;
	void OnLoaded() override;
	void OnUnloaded() override;
	void onUpdate(float _dt) override;

	void OnMouseDown(SGEventMouse* _pMouseEvent) override;
	void OnMouseMove(SGEventMouse* _pMouseEvent) override;
	void OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state) override;

	int getMonsterSpriteSgaIndex() const { return monsterSpriteSgaIndex_; }
	int getMonsterSpriteImgIndex() const { return monsterSpriteImgIndex_; }

	void updateServerSelectionSpriteOpacity(float dt);
	void updateBackgroundGearRotation(float dt);

public:
	// void initServerList();
	// void initChannelList();

	void selectServer(GameServerType_t serverType);
	void selectChannel(int channelIndex);
	void selectChannelTab(ChannelTab tab);

	void enterChannel(GameServerType_t serverType, int channelIndex);

private:
	UISprite* spriteBackgroundGear_;
	UIButton* btnStart_;
	UIButton* btnTerminate_;
	UIButton* btnRefresh_;
	UIToggleButton* toggleTeen_;
	UIToggleButton* toggleAdult_;
	UILabel* labelPage_;

	ServerButton* serverButtons_[GameServerType::Max];
	ChannelButton* channelButtons_[SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT];

	// =========================================

	int monsterSpriteSgaIndex_;
	int monsterSpriteImgIndex_;

	float serverButtonSelectionOpacity_;
	bool serverButtonSelectionOpacityUp_;

	GameServerType_t selectedServer_;
	int selectedChannelIndex_;
	ChannelTab channelTab_;
};
