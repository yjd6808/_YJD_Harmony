/*
 * 작성자: 윤정도
 * 생성일: 8/19/2023 1:07:34 PM
 * =====================
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UI_ChannelSelect.h"

#include <SteinsGate/Client/Define_UI.h>
#include <SteinsGate/Client/Define_Popup.h>
#include <SteinsGate/Client/S_LOBBY.h>

#include <SteinsGate/Common/RectEx.h>
#include <SteinsGate/Common/Vec2Ex.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

#define SG_MONSTER_SPRITE_SGA	"sprite_interface.sga"
#define SG_MONSTER_SPRITE_IMG	"channel_monster.img"

#define SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY	0.8f
#define SG_SERVER_BUTTON_SELECTION_SPRITE_MAX_OPACITY	1.0f
#define SG_BACKGROUND_GEAR_SPEED						2

UI_ChannelSelect::UI_ChannelSelect(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, pSpriteBackgroundGear_(nullptr)
, pBtnStart_{ nullptr }
, pBtnTerminate_{ nullptr }
, pBtnRefresh_{ nullptr }
, pToggleTeen_{ nullptr }
, pToggleBtnAdult_{ nullptr }
, pLabelPage_{ nullptr }
, pServerButtons_{}
, pChannelButtons_{}
, monsterSpriteSgaIndex_(InvalidValue_v)
, monsterSpriteImgIndex_(InvalidValue_v)
, serverButtonSelectionOpacity_(SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY)
, serverButtonSelectionOpacityUp_(true)
, selectedServer_(GameServerType::Max)
, selectedChannelIndex_(InvalidValue_v)
, channelTab_(ChannelTab::Adult)
{
}

UI_ChannelSelect::~UI_ChannelSelect()
{
	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		JCORE_DELETE_SAFE(pChannelButtons_[i]);
	}

	for (int i = 0; i < GameServerType::Max; ++i)
	{
		JCORE_DELETE_SAFE(pServerButtons_[i]);
	}
}


// ========================================================
// 내부 구조체: ServerButton 관련
// ========================================================

UI_ChannelSelect::ServerButton::ServerButton(GameServerType_t type)
: type_(type)
, pGroup_(nullptr)
, pSpriteSelectedBackground_(nullptr)
, pSpriteSelectedText_(nullptr)
, pSpriteSelectedBorder_(nullptr)
, pSpriteNormalText_(nullptr)
, pSpriteNormalBorder_(nullptr)
, pSpriteDisabledText_(nullptr)
, state_(sDisabled)
{
	InitSprites(type);
	SetState(sNormal);
}

void UI_ChannelSelect::ServerButton::InitSprites(GameServerType_t type)
{
	pGroup_ = Core::Contents.UIManager->GetGroup(UI_CHANNEL_SERVER_BUTTON_LIST_GROUP_LUKE + 1000 * type);
	pSpriteServerCharacter_ = Core::Contents.UIManager->GetSprite(UI_CHANNEL_SERVER_CHARACTER_LIST_SPRITE_LUKE + type);
	pSpriteServerCharacter_->setVisible(false);
	pSpriteSelectedBackground_ = pGroup_->GetAtTemplated<UISprite>(0);
	pSpriteSelectedBackground_->setOpacity(SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY);
	pSpriteSelectedText_ = pGroup_->GetAtTemplated<UISprite>(1);
	pSpriteSelectedBorder_ = pGroup_->GetAtTemplated<UISprite>(2);
	pSpriteNormalText_ = pGroup_->GetAtTemplated<UISprite>(3);
	pSpriteNormalBorder_ = pGroup_->GetAtTemplated<UISprite>(4);
	pSpriteDisabledText_ = pGroup_->GetAtTemplated<UISprite>(5);
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

	pSpriteSelectedBackground_->setVisible(true);
	pSpriteSelectedText_->setVisible(true);
	pSpriteSelectedBorder_->setVisible(true);
	pSpriteNormalText_->setVisible(false);
	pSpriteNormalBorder_->setVisible(false);
	pSpriteDisabledText_->setVisible(false);
	pSpriteServerCharacter_->setVisible(true);
}

void UI_ChannelSelect::ServerButton::Normal()
{
	state_ = sNormal;

	pSpriteSelectedBackground_->setVisible(false);
	pSpriteSelectedText_->setVisible(false);
	pSpriteSelectedBorder_->setVisible(false);
	pSpriteNormalText_->setVisible(true);
	pSpriteNormalBorder_->setVisible(true);
	pSpriteDisabledText_->setVisible(false);
	pSpriteServerCharacter_->setVisible(false);
}

void UI_ChannelSelect::ServerButton::Disabled()
{
	state_ = sDisabled;

	pSpriteSelectedBackground_->setVisible(false);
	pSpriteSelectedText_->setVisible(false);
	pSpriteSelectedBorder_->setVisible(false);
	pSpriteNormalText_->setVisible(false);
	pSpriteNormalBorder_->setVisible(false);
	pSpriteDisabledText_->setVisible(true);
	pSpriteServerCharacter_->setVisible(false);
}

bool UI_ChannelSelect::ServerButton::ContainsPoint(const SGVec2& _pos)
{
	if (!pSpriteNormalText_->isVisible())
		return false;


	const SGRect box = pSpriteNormalText_->GetWorldBoundingBox();

	if (box.containsPoint(_pos))
	{
		RectEx::Log(box);
		Vec2Ex::Log(_pos);
	}

	return box.containsPoint(_pos);
}


// ========================================================
// 내부 구조체: ChannelButton 관련
// ========================================================

UI_ChannelSelect::ChannelButton::ChannelButton(int _index)
: index_(_index)
, pToggleBtnEnteranceBackground_{}
, pSpriteMouseOverBorder_(nullptr)
, pSpriteMonsters_{ nullptr }
, pLabelName_(nullptr)
, pLabelDensity_(nullptr)
, state_(sInvisible)
, pInfo_()
{
	InitSprites();
	SetInvisible();
	OnMouseLeave();
}

void UI_ChannelSelect::ChannelButton::InitSprites()
{
	// 6 = SLOT_1_TYPE_0 ~ SLOT_2_TYPE_0 사이의 엘리먼트 갯수가 6개이므로
	pToggleBtnEnteranceBackground_[0] = Core::Contents.UIManager->GetToggleButton(
		UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_SLOT_1_TYPE_0 + 6 * index_);
	pToggleBtnEnteranceBackground_[0]->SetInternalDetailEventEnabled(false);
	pToggleBtnEnteranceBackground_[1] = Core::Contents.UIManager->GetToggleButton(
		UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_SLOT_1_TYPE_1 + 6 * index_);
	pToggleBtnEnteranceBackground_[1]->SetInternalDetailEventEnabled(false);
	pToggleBtnEnteranceBackground_[2] = Core::Contents.UIManager->GetToggleButton(
		UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_SLOT_1_TYPE_2 + 6 * index_);
	pToggleBtnEnteranceBackground_[2]->SetInternalDetailEventEnabled(false);

	pSpriteMouseOverBorder_ = Core::Contents.UIManager->GetSprite(UI_CHANNEL_CHANNEL_LIST_SPRITE_SLOT_1_OVER + 6 * index_);
	pLabelName_ = Core::Contents.UIManager->GetLabel(UI_CHANNEL_CHANNEL_LIST_LABEL_SLOT_1_NAME + 6 * index_);
	pLabelDensity_ = Core::Contents.UIManager->GetLabel(UI_CHANNEL_CHANNEL_LIST_LABEL_SLOT_1_DENSITY + 6 * index_);
	pLabelDensity_->source()->enableOutline(Color4B::BLACK, 3);
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
	const EnteranceType_t entranceType = pInfo_ ? pInfo_->enteranceType_ : EnteranceType::Normal;

	state_ = sNormal;

	for (int i = 0; i < EnteranceType::Max; ++i)
	{
		pToggleBtnEnteranceBackground_[i]->setVisible(i == entranceType);
		pToggleBtnEnteranceBackground_[i]->SetToggleState(ToggleState::eNormal);
	}

	if (pSpriteMonsters_[0])
		pSpriteMonsters_[0]->setVisible(false);
	if (pSpriteMonsters_[1])
		pSpriteMonsters_[1]->setVisible(true);

	pLabelName_->setVisible(true);
	pLabelDensity_->setVisible(true);
}

void UI_ChannelSelect::ChannelButton::SetSelected()
{
	const EnteranceType_t entranceType = pInfo_ ? pInfo_->enteranceType_ : EnteranceType::Normal;

	state_ = sSelected;

	for (int i = 0; i < EnteranceType::Max; ++i)
	{
		pToggleBtnEnteranceBackground_[i]->setVisible(i == entranceType);
		pToggleBtnEnteranceBackground_[i]->SetToggleState(ToggleState::eToggled);
	}

	if (pSpriteMonsters_[0])
		pSpriteMonsters_[0]->setVisible(true);
	if (pSpriteMonsters_[1])
		pSpriteMonsters_[1]->setVisible(false);
	pLabelName_->setVisible(true);
	pLabelDensity_->setVisible(true);
}

void UI_ChannelSelect::ChannelButton::SetInvisible()
{
	state_ = sInvisible;

	for (int i = 0; i < EnteranceType::Max; ++i)
	{
		pToggleBtnEnteranceBackground_[i]->setVisible(false);
	}

	if (pSpriteMonsters_[0])
		pSpriteMonsters_[0]->setVisible(false);
	if (pSpriteMonsters_[1])
		pSpriteMonsters_[1]->setVisible(false);
	pLabelName_->setVisible(false);
	pLabelDensity_->setVisible(false);
}

void UI_ChannelSelect::ChannelButton::OnMouseOver()
{
	pSpriteMouseOverBorder_->setVisible(true);
}

void UI_ChannelSelect::ChannelButton::OnMouseLeave()
{
	pSpriteMouseOverBorder_->setVisible(false);
}

void UI_ChannelSelect::ChannelButton::InitMonsterSprites(UI_ChannelSelect* _pRootGroup)
{
	if (pInfo_ == nullptr)
	{
		_LogWarn_("ChannelButton::initMonsterSprites");
	}

	UIGroup* pParent = Core::Contents.UIManager->GetGroup(UI_CHANNEL_GROUP_CHANNEL_LIST);

	int iSelectedSprite = pInfo_ ? pInfo_->selectedSpriteIndex_ : InvalidValue_v;
	int iNormalSprite = pInfo_ ? pInfo_->normalSpriteIndex_ : InvalidValue_v;

	if (pSpriteMonsters_[0] == nullptr)
	{
		UISpriteInfo* pSelectedInfo = dbg_new UISpriteInfo();
		pSelectedInfo->sga_ = _pRootGroup->GetMonsterSpriteSgaIndex();
		pSelectedInfo->img_ = _pRootGroup->GetMonsterSpriteImgIndex();
		pSelectedInfo->sprite_ = iSelectedSprite;

		pSpriteMonsters_[0] = UISprite::Create(_pRootGroup, pParent, pSelectedInfo, true);
		pSpriteMonsters_[0]->SetDeveloperCreated(true);
		pSpriteMonsters_[0]->Load();
		pSpriteMonsters_[0]->SetRelativePosition(pToggleBtnEnteranceBackground_[0], 5, 5, HAlignment::Left, VAlignment::Bottom);


		UISpriteInfo* pNormalInfo = dbg_new UISpriteInfo(*pSelectedInfo);
		pNormalInfo->sprite_ = iNormalSprite;

		pSpriteMonsters_[1] = UISprite::Create(_pRootGroup, pParent, pNormalInfo, true);
		pSpriteMonsters_[1]->SetDeveloperCreated(true);
		pSpriteMonsters_[1]->Load();
		pSpriteMonsters_[1]->SetRelativePosition(pToggleBtnEnteranceBackground_[0], 5, 5, HAlignment::Left, VAlignment::Bottom);


		pParent->AddChild(pSpriteMonsters_[0]);
		pParent->AddChild(pSpriteMonsters_[1]);
	}
	else
	{
		pSpriteMonsters_[0]->GetInfo()->sprite_ = pInfo_->selectedSpriteIndex_;
		pSpriteMonsters_[0]->Reload();
		pSpriteMonsters_[1]->GetInfo()->sprite_ = pInfo_->normalSpriteIndex_;
		pSpriteMonsters_[1]->Reload();
	}
}

void UI_ChannelSelect::ChannelButton::SetChannelInfo(const LobbyChannelInfo& channelData, char _sequence)
{
	pInfo_ = Core::DataManager->GetChannelInfo(channelData.type_);
	const SGString& szFmtName = pLabelName_->getInfo()->text_; // szFmtName = "ch%02d.%s %c"
	pLabelName_->setText(StringUtils::format(szFmtName.Source(), channelData.number_, pInfo_->name_.Source(), _sequence));

	pLabelDensity_->setText(ChannelDensity::Name[channelData.desity_]);
	pLabelDensity_->setColor(ChannelDensityEx::Color[channelData.desity_]);
}

bool UI_ChannelSelect::ChannelButton::ContainsPoint(const SGVec2& _pos)
{
	for (int i = 0; i < EnteranceType::Max; ++i)
	{
		if (pToggleBtnEnteranceBackground_[i]->isVisible() && pToggleBtnEnteranceBackground_[i]->GetWorldBoundingBox().
			containsPoint(_pos))
		{
			return true;
		}
	}
	return false;
}

void UI_ChannelSelect::OnInit()
{
	monsterSpriteSgaIndex_ = Core::Contents.PackManager->GetPackIndexDefault(SG_MONSTER_SPRITE_SGA);
	monsterSpriteImgIndex_ = Core::Contents.PackManager->GetImgIndexUnsafe(
		monsterSpriteSgaIndex_, SG_MONSTER_SPRITE_IMG);

	if (monsterSpriteSgaIndex_ == InvalidValue_v)
	{
		_LogWarn_("채널 몬스터 SgaIndex 정보를 얻는데 실패했습니다.");
	}

	if (monsterSpriteImgIndex_ == InvalidValue_v)
	{
		_LogWarn_("채널 몬스터 ImngIndex 정보를 얻는데 실패했습니다.");
	}

	pSpriteBackgroundGear_ = Core::Contents.UIManager->GetSprite(UI_CHANNEL_SPRITE_BACKGROUND_GEAR);
	pBtnStart_ = Core::Contents.UIManager->GetButton(UI_CHANNEL_BUTTON_START);
	pBtnTerminate_ = Core::Contents.UIManager->GetButton(UI_CHANNEL_BUTTON_TERMINATE);

	pBtnRefresh_ = Core::Contents.UIManager->GetButton(UI_CHANNEL_CHANNEL_LIST_BUTTON_REFRESH);
	pToggleTeen_ = Core::Contents.UIManager->GetToggleButton(UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_TEEN);
	pToggleBtnAdult_ = Core::Contents.UIManager->GetToggleButton(UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_ADULT);
	pLabelPage_ = Core::Contents.UIManager->GetLabel(UI_CHANNEL_CHANNEL_LIST_LABEL_PAGE);

	for (int i = 0; i < GameServerType::Max; ++i)
	{
		pServerButtons_[i] = dbg_new ServerButton{ (GameServerType_t)i };
	}

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		pChannelButtons_[i] = dbg_new ChannelButton(i);
	}
}

void UI_ChannelSelect::OnLoaded()
{
	SelectChannelTab(ChannelTab::Teen);

	SGSprite* pGearSprite = pSpriteBackgroundGear_->Source();
	pGearSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	pGearSprite->setPosition(Core::ClientInfo->uiResolutionSize_.width / 2.0f,
	                         Core::ClientInfo->uiResolutionSize_.height / 2.0f);

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = pChannelButtons_[i];
		pButton->SetState(ChannelButton::sNormal);
	}

	LobbyChannelInfo a[4]{
		{ 1, 1, ChannelDensity::Free },
		{ 2, 11, ChannelDensity::Normal },
		{ 3, 13, ChannelDensity::Busy },
		{ 4, 14, ChannelDensity::Full },
	};

	pChannelButtons_[0]->SetChannelInfo(a[0], 'A');
	pChannelButtons_[0]->InitMonsterSprites(this);
	pChannelButtons_[0]->SetState(ChannelButton::sSelected);

	pChannelButtons_[1]->SetChannelInfo(a[1], 'A');
	pChannelButtons_[1]->InitMonsterSprites(this);
	pChannelButtons_[1]->SetState(ChannelButton::sNormal);

	pChannelButtons_[2]->SetChannelInfo(a[2], 'A');
	pChannelButtons_[2]->InitMonsterSprites(this);
	pChannelButtons_[2]->SetState(ChannelButton::sNormal);

	pChannelButtons_[3]->SetChannelInfo(a[3], 'A');
	pChannelButtons_[3]->InitMonsterSprites(this);
	pChannelButtons_[3]->SetState(ChannelButton::sNormal);
}

void UI_ChannelSelect::OnUnloaded()
{
	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = pChannelButtons_[i];
		pButton->pSpriteMonsters_[0] = nullptr;
		pButton->pSpriteMonsters_[1] = nullptr;
	}
}

void UI_ChannelSelect::OnUpdate(float _dt)
{
	UpdateServerSelectionSpriteOpacity(_dt);
	UpdateBackgroundGearRotation(_dt);
}

void UI_ChannelSelect::OnMouseDown(SGEventMouse* _pMouseEvent)
{
	const SGVec2 cursorPos = _pMouseEvent->getCursorPos();

	for (int i = 0; i < GameServerType::Max; ++i)
	{
		ServerButton* pButton = pServerButtons_[i];
		if (pButton->state_ == ServerButton::sNormal && pButton->ContainsPoint(cursorPos))
		{
			SelectServer(GameServerType_t(i));
			break;
		}
	}

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = pChannelButtons_[i];
		if (pButton->state_ == ChannelButton::sNormal && pButton->ContainsPoint(cursorPos))
		{
			SelectChannel(i);
			break;
		}
	}
}

void UI_ChannelSelect::OnMouseMove(SGEventMouse* _pMouseEvent)
{
	const SGVec2 cursorPos = _pMouseEvent->getCursorPos();

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = pChannelButtons_[i];

		if (pButton->state_ == ChannelButton::sInvisible)
			continue;

		if (pButton->ContainsPoint(cursorPos))
		{
			pButton->OnMouseOver();
		}
		else
		{
			pButton->OnMouseLeave();
		}
	}
}

void UI_ChannelSelect::OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state)
{
	JCORE_UNUSED(_state);

	switch (_pToggleBtn->GetCode())
	{
	case UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_TEEN:
		SelectChannelTab(ChannelTab::Teen);
		break;
	case UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_ADULT:
		SelectChannelTab(ChannelTab::Adult);
		break;
	default:
		break;
	}
}

void UI_ChannelSelect::UpdateServerSelectionSpriteOpacity(float _dt)
{
	if (selectedServer_ == GameServerType::Max)
	{
		return;
	}

	if (serverButtonSelectionOpacityUp_)
		serverButtonSelectionOpacity_ += _dt * 0.3f;
	else
		serverButtonSelectionOpacity_ -= _dt * 0.3f;

	if (serverButtonSelectionOpacity_ > SG_SERVER_BUTTON_SELECTION_SPRITE_MAX_OPACITY)
	{
		serverButtonSelectionOpacity_ = SG_SERVER_BUTTON_SELECTION_SPRITE_MAX_OPACITY;
		serverButtonSelectionOpacityUp_ = false;
	}
	else if (serverButtonSelectionOpacity_ < SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY)
	{
		serverButtonSelectionOpacity_ = SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY;
		serverButtonSelectionOpacityUp_ = true;
	}

	pServerButtons_[selectedServer_]->pSpriteSelectedBackground_->setOpacityF(serverButtonSelectionOpacity_);
}

void UI_ChannelSelect::UpdateBackgroundGearRotation(float _dt)
{
	SGSprite* pGearSprite = pSpriteBackgroundGear_->Source();
	pGearSprite->setRotation(pGearSprite->getRotation() + _dt * SG_BACKGROUND_GEAR_SPEED);
}

void UI_ChannelSelect::SelectServer(GameServerType_t _serverType)
{
	if (selectedServer_ == _serverType)
		return;

	selectedServer_ = _serverType;

	for (int i = 0; i < GameServerType::Max; ++i)
	{
		ServerButton* pButton = pServerButtons_[i];
		if (i == _serverType)
			pButton->SetState(ServerButton::sSelected);
		else if (pButton->state_ == ServerButton::sSelected)
			pButton->SetState(ServerButton::sNormal);
	}

	S_LOBBY::SEND_CLO_LoadChannelInfo(_serverType);
}

void UI_ChannelSelect::SelectChannel(int _channelIndex)
{
	if (selectedChannelIndex_ == _channelIndex)
	{
		return;
	}

	selectedChannelIndex_ = _channelIndex;

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = pChannelButtons_[i];
		if (i == _channelIndex)
			pButton->SetState(ChannelButton::sSelected);
		else if (pButton->state_ == ChannelButton::sSelected)
			pButton->SetState(ChannelButton::sNormal);
	}
}

void UI_ChannelSelect::SelectChannelTab(ChannelTab _tab)
{
	channelTab_ = _tab;

	if (channelTab_ == ChannelTab::Teen)
	{
		pToggleBtnAdult_->SetToggleState(ToggleState::eNormal);
		pToggleTeen_->SetToggleState(ToggleState::eToggled);
	}
	else if (channelTab_ == ChannelTab::Adult)
	{
		pToggleBtnAdult_->SetToggleState(ToggleState::eToggled);
		pToggleTeen_->SetToggleState(ToggleState::eNormal);
	}
}

void UI_ChannelSelect::EnterChannel(GameServerType_t _serverType, int _channelIndex)
{
	JCORE_UNUSED(_serverType);
	JCORE_UNUSED(_channelIndex);
}
