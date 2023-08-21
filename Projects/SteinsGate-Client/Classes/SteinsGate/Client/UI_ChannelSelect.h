/*
 * 작성자: 윤정도
 * 생성일: 8/19/2023 1:20:08 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIMasterGroup.h>

#define SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT	10

class UI_ChannelSelect : public UIMasterGroup
{
public:
	UI_ChannelSelect(UIGroupInfo* groupInfo);
	~UI_ChannelSelect() override;

	enum class ChannelTab
	{
		Teen,		// 청소년
		Adult		// 성인
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
	void onInit() override;
	void onLoaded() override;
	void onUnloaded() override;
	void onUpdate(float dt) override;

	void onMouseDown(SGEventMouse* mouseEvent) override;
	void onMouseMove(SGEventMouse* mouseEvent) override;
	void onToggleStateChanged(UIToggleButton* toggleBtn, ToggleState state) override;

	int getMonsterSpriteSgaIndex() const { return m_iMonsterSpriteSgaIndex; }
	int getMonsterSpriteImgIndex() const { return m_iMonsterSpriteImgIndex; }

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

	UISprite* m_pSpriteBackgroundGear;
	UIButton* m_pBtnStart;
	UIButton* m_pBtnTerminate;
	UIButton* m_pBtnRefresh;
	UIToggleButton* m_pTBtnTeen;
	UIToggleButton* m_pTBtnAdult;
	UILabel* m_pLabelPage;

	ServerButton* m_arrServerButtonList[GameServerType::Max];
	ChannelButton* m_arrChannelButtonList[SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT];

	// =========================================

	int m_iMonsterSpriteSgaIndex;
	int m_iMonsterSpriteImgIndex;

	float m_fServerButtonSelectionOpacity;
	bool m_bServerButtonSelectionOpacityUp;

	GameServerType_t m_eSelectedServer;
	int m_iSelectedChannelIndex;
	ChannelTab m_eChannelTab;
};


