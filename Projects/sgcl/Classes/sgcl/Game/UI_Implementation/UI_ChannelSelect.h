/*
 * 작성자: 윤정도
 * =====================
 * 채널 선택 화면 (sgui 기반 재구현)
 * SGA 장식 스프라이트 의존을 제거하고 전부 테마 기반 컨트롤로 구성한다.
 */

#pragma once

#include "sg/Struct/SteinsGate_ChannelBase.h"
#include "sgcl/Game/UI/Sgui.h"

#define SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT	10

struct ChannelInfo;
class UI_ChannelSelect : public sgui::Window
{
public:
	virtual ~UI_ChannelSelect() override;

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

		void SetState(State _state);
		void Select();
		void Normal();
		void Disabled();

		GameServerType_t type_;
		sgui::ToggleButton* pButton_;
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

		void SetState(State _state);
		void SetNormal();
		void SetSelected();
		void SetInvisible();

		void SetChannelInfo(const LobbyChannelInfo& _channelInfo, char _sequence);

		int index_;
		sgui::ToggleButton* pFrame_;
		sgui::TextBlock* pLabelName_;
		sgui::TextBlock* pLabelDensity_;
		State state_;
		ChannelInfo* pInfo_;
	};

	virtual void OnInit(const jc::CDataMap<>& _param) override;
	virtual void OnLoaded() override;

	void SelectServer(GameServerType_t _serverType);
	void SelectChannel(int _channelIndex);
	void SelectChannelTab(ChannelTab _tab);

	void EnterChannel(GameServerType_t _serverType, int _channelIndex);

private:
	void InitFromXml(const jc::CDataMap<>& _param);
	void BuildServerButtons(sgui::Panel* _container);
	void BuildChannelButtons(sgui::Grid* _container);

	sgui::Button* pBtnStart_ = nullptr;
	sgui::Button* pBtnTerminate_ = nullptr;
	sgui::Button* pBtnRefresh_ = nullptr;
	sgui::ToggleButton* pToggleTeen_ = nullptr;
	sgui::ToggleButton* pToggleBtnAdult_ = nullptr;
	sgui::TextBlock* pLabelPage_ = nullptr;

	ServerButton* pServerButtons_[GameServerType::Max] = {};
	ChannelButton* pChannelButtons_[SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT] = {};

	GameServerType_t selectedServer_ = GameServerType::Max;
	int selectedChannelIndex_ = InvalidValue_v;
	ChannelTab channelTab_ = ChannelTab::Adult;
};
