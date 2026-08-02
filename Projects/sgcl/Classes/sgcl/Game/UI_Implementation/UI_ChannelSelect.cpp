/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_ChannelSelect.h"

#include "sg/Util/DescLoaderMgr.h"
#include "sg/Struct/SteinsGate_Channel.h"

#include "sgcl/Define/Define_Popup.h"
#include "sgcl/Net/Send/S_LOBBY.h"
#include "sgcl/Game/Contents/Contents.h"
#include "sgcl/Scene/Scene_World.h"

USING_NS_CC;
USING_NS_JC;

using namespace sgui;

static const char* kServerNames[] = { "luke", "hilder", "siroco", "prey", "kasias", "diregie", "cain", "seria" };

UI_ChannelSelect::~UI_ChannelSelect()
{
	for (int idx = 0; idx < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++idx)
	{
		JC_DELETE_SAFE(pChannelButtons_[idx]);
	}

	for (int idx = 0; idx < GameServerType::Max; ++idx)
	{
		JC_DELETE_SAFE(pServerButtons_[idx]);
	}
}

// =========================================================================================
// ServerButton
// =========================================================================================

UI_ChannelSelect::ServerButton::ServerButton(GameServerType_t _type)
: type_(_type)
, pButton_(nullptr)
, state_(sDisabled)
{
}

void UI_ChannelSelect::ServerButton::SetState(State _state)
{
	switch (_state)
	{
	case sSelected: Select();
		break;
	case sNormal: Normal();
		break;
	case sDisabled: Disabled();
		break;
	}
}

void UI_ChannelSelect::ServerButton::Select()
{
	state_ = sSelected;
	pButton_->SetIsEnabled(true);
	pButton_->SetIsChecked(true);
}

void UI_ChannelSelect::ServerButton::Normal()
{
	state_ = sNormal;
	pButton_->SetIsEnabled(true);
	pButton_->SetIsChecked(false);
}

void UI_ChannelSelect::ServerButton::Disabled()
{
	state_ = sDisabled;
	pButton_->SetIsChecked(false);
	pButton_->SetIsEnabled(false);
}

// =========================================================================================
// ChannelButton
// =========================================================================================

UI_ChannelSelect::ChannelButton::ChannelButton(int _index)
: index_(_index)
, pFrame_(nullptr)
, pLabelName_(nullptr)
, pLabelDensity_(nullptr)
, state_(sInvisible)
, pInfo_()
{
}

void UI_ChannelSelect::ChannelButton::SetState(State _state)
{
	switch (_state)
	{
	case sNormal: SetNormal();
		break;
	case sSelected: SetSelected();
		break;
	case sInvisible: SetInvisible();
		break;
	}
}

void UI_ChannelSelect::ChannelButton::SetNormal()
{
	state_ = sNormal;
	pFrame_->SetVisibility(Visibility::Visible);
	pFrame_->SetIsChecked(false);
}

void UI_ChannelSelect::ChannelButton::SetSelected()
{
	state_ = sSelected;
	pFrame_->SetVisibility(Visibility::Visible);
	pFrame_->SetIsChecked(true);
}

void UI_ChannelSelect::ChannelButton::SetInvisible()
{
	state_ = sInvisible;
	pFrame_->SetIsChecked(false);
	pFrame_->SetVisibility(Visibility::Hidden);
}

void UI_ChannelSelect::ChannelButton::SetChannelInfo(const LobbyChannelInfo& _channelInfo, char _sequence)
{
	pInfo_ = g_cDescMgr.GetChannelInfo(_channelInfo.type_);
	pLabelName_->SetText(StringUtils::format("%d. %s-%c", _channelInfo.number_, pInfo_->name_.Source(), _sequence).c_str());

	pLabelDensity_->SetText(ChannelDensity::Name[_channelInfo.desity_]);

	const cc::Color3B& densityColor = ChannelDensity::Color[_channelInfo.desity_];
	pLabelDensity_->SetForeground(UIColorF{
		densityColor.r / 255.0f,
		densityColor.g / 255.0f,
		densityColor.b / 255.0f,
		1.0f });
}

// =========================================================================================
// UI_ChannelSelect
// =========================================================================================

void UI_ChannelSelect::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	// ==================== XML 필수 (layout/ui_channel.xml) ====================
	// sg-main 방식: XML로 로드된 Element를 OnInit 단에서 FindElementByName으로 멤버에 할당한다.
	// 서버/채널 버튼은 동적 데이터이므로 XML의 컨테이너 패널을 찾아 그 위에 생성한다.
	InitFromXml(_param);
}

void UI_ChannelSelect::InitFromXml(const CDataMap<>& _param)
{
	UNUSED(_param);

	pBtnStart_ = FindElementByName<Button>("start");
	pBtnTerminate_ = FindElementByName<Button>("terminate");
	pBtnRefresh_ = FindElementByName<Button>("refresh");
	pToggleTeen_ = FindElementByName<ToggleButton>("teen");
	pToggleBtnAdult_ = FindElementByName<ToggleButton>("adult");
	pLabelPage_ = FindElementByName<TextBlock>("page");

	if (pBtnStart_)
	{
		pBtnStart_->SetClickCallback([this](ButtonBase*)
		{
			if (selectedServer_ != GameServerType::Max && selectedChannelIndex_ != InvalidValue_v)
			{
				EnterChannel(selectedServer_, selectedChannelIndex_);
			}
		});
	}

	if (pBtnRefresh_)
	{
		pBtnRefresh_->SetClickCallback([this](ButtonBase*)
		{
			if (selectedServer_ != GameServerType::Max)
			{
				S_LOBBY::SEND_CLO_LoadChannelInfo(selectedServer_);
			}
		});
	}

	if (pBtnTerminate_)
	{
		pBtnTerminate_->SetClickCallback([](ButtonBase*) { g_cWorld.Terminate(); });
	}

	if (pToggleTeen_)
	{
		pToggleTeen_->SetClickCallback([this](ButtonBase*) { SelectChannelTab(ChannelTab::Teen); });
	}

	if (pToggleBtnAdult_)
	{
		pToggleBtnAdult_->SetClickCallback([this](ButtonBase*) { SelectChannelTab(ChannelTab::Adult); });
	}

	// 동적 서버/채널 버튼은 XML에 정의된 컨테이너 패널 위에 생성한다.
	if (Panel* pServerContainer = FindElementByName<Panel>("server_list"))
	{
		BuildServerButtons(pServerContainer);
	}

	if (Grid* pChannelContainer = FindElementByName<Grid>("channel_list"))
	{
		BuildChannelButtons(pChannelContainer);
		_LogDebug_("[UI_ChannelSelect] channel_list Grid col=%d row=%d children=%d",
			pChannelContainer->GetColumnCount(), pChannelContainer->GetRowCount(), (int)pChannelContainer->getChildrenCount());
	}
	else
	{
		_LogWarn_("[UI_ChannelSelect] channel_list 컨테이너를 찾지 못함");
	}

	_LogDebug_("[UI_ChannelSelect] OnInit(XML) start=%p terminate=%p refresh=%p teen=%p adult=%p page=%p",
		pBtnStart_, pBtnTerminate_, pBtnRefresh_, pToggleTeen_, pToggleBtnAdult_, pLabelPage_);
}

void UI_ChannelSelect::BuildServerButtons(sgui::Panel* _container)
{
	for (int idx = 0; idx < GameServerType::Max; ++idx)
	{
		pServerButtons_[idx] = dbg_new ServerButton{ (GameServerType_t)idx };

		ToggleButton* pToggle = ToggleButton::Create(kServerNames[idx]);
		pToggle->setName(kServerNames[idx]);
		pToggle->SetMargin(Thickness(3.0f));

		const GameServerType_t serverType = (GameServerType_t)idx;
		pToggle->SetClickCallback([this, serverType](ButtonBase*) { SelectServer(serverType); });

		pServerButtons_[idx]->pButton_ = pToggle;
		pServerButtons_[idx]->SetState(ServerButton::sNormal);
		_container->AddChild(pToggle);
	}
}

void UI_ChannelSelect::BuildChannelButtons(sgui::Grid* _grid)
{
	for (int idx = 0; idx < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++idx)
	{
		pChannelButtons_[idx] = dbg_new ChannelButton(idx);
		ChannelButton* pBtn = pChannelButtons_[idx];

		StackPanel* pContentStack = StackPanel::Create(Orientation::Vertical);
		pContentStack->SetSpacing(4.0f);

		pBtn->pLabelName_ = TextBlock::Create("");
		pBtn->pLabelName_->setName(jc::StringUtil::Format("slot_%d_name", idx + 1).Source());
		pContentStack->AddChild(pBtn->pLabelName_);

		pBtn->pLabelDensity_ = TextBlock::Create("");
		pBtn->pLabelDensity_->setName(jc::StringUtil::Format("slot_%d_density", idx + 1).Source());
		pContentStack->AddChild(pBtn->pLabelDensity_);

		ToggleButton* pFrame = ToggleButton::Create(nullptr);
		pFrame->setName(jc::StringUtil::Format("slot_%d", idx + 1).Source());
		pFrame->SetContent(pContentStack);
		pFrame->SetMargin(Thickness(3.0f));

		// XML에 정의된 2열 ColumnDefinition 기준으로 부착 속성 배치
		pFrame->SetGridColumn(idx % 2);
		pFrame->SetGridRow(idx / 2);

		const int channelIndex = idx;
		pFrame->SetClickCallback([this, channelIndex](ButtonBase*) { SelectChannel(channelIndex); });

		pBtn->pFrame_ = pFrame;
		pBtn->SetInvisible();
		_grid->AddChild(pFrame);
	}
}

void UI_ChannelSelect::OnLoaded()
{
	SelectChannelTab(ChannelTab::Teen);

	for (int idx = 0; idx < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++idx)
	{
		pChannelButtons_[idx]->SetState(ChannelButton::sNormal);
	}

	// 임시 테스트 데이터 (원본 로직 유지)
	LobbyChannelInfo a[4]{
		{ 1, 1, ChannelDensity::Free },
		{ 2, 11, ChannelDensity::Normal },
		{ 3, 13, ChannelDensity::Busy },
		{ 4, 14, ChannelDensity::Full },
	};

	pChannelButtons_[0]->SetChannelInfo(a[0], 'A');
	pChannelButtons_[0]->SetState(ChannelButton::sSelected);

	pChannelButtons_[1]->SetChannelInfo(a[1], 'A');
	pChannelButtons_[1]->SetState(ChannelButton::sNormal);

	pChannelButtons_[2]->SetChannelInfo(a[2], 'A');
	pChannelButtons_[2]->SetState(ChannelButton::sNormal);

	pChannelButtons_[3]->SetChannelInfo(a[3], 'A');
	pChannelButtons_[3]->SetState(ChannelButton::sNormal);

	selectedChannelIndex_ = 0;

	for (int idx = 4; idx < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++idx)
	{
		pChannelButtons_[idx]->SetState(ChannelButton::sInvisible);
	}
}

void UI_ChannelSelect::SelectServer(GameServerType_t _serverType)
{
	if (selectedServer_ == _serverType)
	{
		// 토글 되돌림 방지
		pServerButtons_[_serverType]->SetState(ServerButton::sSelected);
		return;
	}

	selectedServer_ = _serverType;

	for (int idx = 0; idx < GameServerType::Max; ++idx)
	{
		ServerButton* pButton = pServerButtons_[idx];
		if (idx == _serverType)
		{
			pButton->SetState(ServerButton::sSelected);
		}
		else if (pButton->state_ == ServerButton::sSelected)
		{
			pButton->SetState(ServerButton::sNormal);
		}
	}

	S_LOBBY::SEND_CLO_LoadChannelInfo(_serverType);
}

void UI_ChannelSelect::SelectChannel(int _channelIndex)
{
	if (selectedChannelIndex_ == _channelIndex)
	{
		// 토글 되돌림 방지
		pChannelButtons_[_channelIndex]->SetState(ChannelButton::sSelected);
		return;
	}

	selectedChannelIndex_ = _channelIndex;

	for (int idx = 0; idx < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++idx)
	{
		ChannelButton* pButton = pChannelButtons_[idx];
		if (idx == _channelIndex)
		{
			pButton->SetState(ChannelButton::sSelected);
		}
		else if (pButton->state_ == ChannelButton::sSelected)
		{
			pButton->SetState(ChannelButton::sNormal);
		}
	}
}

void UI_ChannelSelect::SelectChannelTab(ChannelTab _tab)
{
	channelTab_ = _tab;

	if (pToggleTeen_)
	{
		pToggleTeen_->SetIsChecked(channelTab_ == ChannelTab::Teen);
	}
	if (pToggleBtnAdult_)
	{
		pToggleBtnAdult_->SetIsChecked(channelTab_ == ChannelTab::Adult);
	}
}

void UI_ChannelSelect::EnterChannel(GameServerType_t _serverType, int _channelIndex)
{
	UNUSED(_serverType);
	UNUSED(_channelIndex);
}

REGISTER_WINDOW(UI_ChannelSelect, "ui_channel")
