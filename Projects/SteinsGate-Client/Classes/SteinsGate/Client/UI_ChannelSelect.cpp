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
, spriteBackgroundGear_(nullptr)
, btnStart_{ nullptr }
, btnTerminate_{ nullptr }
, btnRefresh_{ nullptr }
, toggleTeen_{ nullptr }
, toggleAdult_{ nullptr }
, labelPage_{ nullptr }
, serverButtons_{}
, channelButtons_{}
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
		JCORE_DELETE_SAFE(channelButtons_[i]);
	}

	for (int i = 0; i < GameServerType::Max; ++i)
	{
		JCORE_DELETE_SAFE(serverButtons_[i]);
	}
}


// ========================================================
// 내부 구조체: ServerButton 관련
// ========================================================

UI_ChannelSelect::ServerButton::ServerButton(GameServerType_t type)
: Type(type)
, Group(nullptr)
, SpriteSelectedBackground(nullptr)
, SpriteSelectedText(nullptr)
, SpriteSelectedBorder(nullptr)
, SpriteNormalText(nullptr)
, SpriteNormalBorder(nullptr)
, SpriteDisabledText(nullptr)
, State_(Disabled)
{
	initSprites(type);
	setState(Normal);
}

void UI_ChannelSelect::ServerButton::initSprites(GameServerType_t type)
{
	Group = Core::Contents.UIManager->getGroup(UI_CHANNEL_SERVER_BUTTON_LIST_GROUP_LUKE + 1000 * type);
	SpriteServerCharacter = Core::Contents.UIManager->getSprite(UI_CHANNEL_SERVER_CHARACTER_LIST_SPRITE_LUKE + type);
	SpriteServerCharacter->setVisible(false);
	SpriteSelectedBackground = Group->getAtTemplated<UISprite>(0);
	SpriteSelectedBackground->setOpacity(SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY);
	SpriteSelectedText = Group->getAtTemplated<UISprite>(1);
	SpriteSelectedBorder = Group->getAtTemplated<UISprite>(2);
	SpriteNormalText = Group->getAtTemplated<UISprite>(3);
	SpriteNormalBorder = Group->getAtTemplated<UISprite>(4);
	SpriteDisabledText = Group->getAtTemplated<UISprite>(5);
}

void UI_ChannelSelect::ServerButton::setState(State state)
{
	switch (state)
	{
	case Selected: select();
		break;
	case Normal: normal();
		break;
	case Disabled: disabled();
		break;
	}
}

void UI_ChannelSelect::ServerButton::select()
{
	State_ = Selected;

	SpriteSelectedBackground->setVisible(true);
	SpriteSelectedText->setVisible(true);
	SpriteSelectedBorder->setVisible(true);
	SpriteNormalText->setVisible(false);
	SpriteNormalBorder->setVisible(false);
	SpriteDisabledText->setVisible(false);
	SpriteServerCharacter->setVisible(true);
}

void UI_ChannelSelect::ServerButton::normal()
{
	State_ = Normal;

	SpriteSelectedBackground->setVisible(false);
	SpriteSelectedText->setVisible(false);
	SpriteSelectedBorder->setVisible(false);
	SpriteNormalText->setVisible(true);
	SpriteNormalBorder->setVisible(true);
	SpriteDisabledText->setVisible(false);
	SpriteServerCharacter->setVisible(false);
}

void UI_ChannelSelect::ServerButton::disabled()
{
	State_ = Disabled;

	SpriteSelectedBackground->setVisible(false);
	SpriteSelectedText->setVisible(false);
	SpriteSelectedBorder->setVisible(false);
	SpriteNormalText->setVisible(false);
	SpriteNormalBorder->setVisible(false);
	SpriteDisabledText->setVisible(true);
	SpriteServerCharacter->setVisible(false);
}

bool UI_ChannelSelect::ServerButton::containsPoint(const SGVec2& pos)
{
	if (!SpriteNormalText->isVisible())
		return false;


	const SGRect box = SpriteNormalText->GetWorldBoundingBox();

	if (box.containsPoint(pos))
	{
		RectEx::Log(box);
		Vec2Ex::Log(pos);
	}

	return box.containsPoint(pos);
}


// ========================================================
// 내부 구조체: ChannelButton 관련
// ========================================================

UI_ChannelSelect::ChannelButton::ChannelButton(int index)
: Index(index)
, TBtnEnteranceBackground{}
, SpriteMouseOverBorder(nullptr)
, SpriteMonster{ nullptr }
, LabelName(nullptr)
, LabelDensity(nullptr)
, State_(Invisible)
, Info()
{
	initSprites();
	invisible();
	mouseLeave();
}

void UI_ChannelSelect::ChannelButton::initSprites()
{
	// 6 = SLOT_1_TYPE_0 ~ SLOT_2_TYPE_0 사이의 엘리먼트 갯수가 6개이므로
	TBtnEnteranceBackground[0] = Core::Contents.UIManager->getToggleButton(
		UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_SLOT_1_TYPE_0 + 6 * Index);
	TBtnEnteranceBackground[0]->SetInternalDetailEventEnabled(false);
	TBtnEnteranceBackground[1] = Core::Contents.UIManager->getToggleButton(
		UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_SLOT_1_TYPE_1 + 6 * Index);
	TBtnEnteranceBackground[1]->SetInternalDetailEventEnabled(false);
	TBtnEnteranceBackground[2] = Core::Contents.UIManager->getToggleButton(
		UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_SLOT_1_TYPE_2 + 6 * Index);
	TBtnEnteranceBackground[2]->SetInternalDetailEventEnabled(false);

	SpriteMouseOverBorder = Core::Contents.UIManager->getSprite(UI_CHANNEL_CHANNEL_LIST_SPRITE_SLOT_1_OVER + 6 * Index);
	LabelName = Core::Contents.UIManager->getLabel(UI_CHANNEL_CHANNEL_LIST_LABEL_SLOT_1_NAME + 6 * Index);
	LabelDensity = Core::Contents.UIManager->getLabel(UI_CHANNEL_CHANNEL_LIST_LABEL_SLOT_1_DENSITY + 6 * Index);
	LabelDensity->source()->enableOutline(Color4B::BLACK, 3);
}

void UI_ChannelSelect::ChannelButton::setState(State state)
{
	switch (state)
	{
	case Normal: normal();
		break;
	case Selected: selected();
		break;
	case Invisible: invisible();
		break;
	}
}

void UI_ChannelSelect::ChannelButton::normal()
{
	const EnteranceType_t entranceType = Info ? Info->enteranceType_ : EnteranceType::Normal;

	State_ = Normal;

	for (int i = 0; i < EnteranceType::Max; ++i)
	{
		TBtnEnteranceBackground[i]->setVisible(i == entranceType);
		TBtnEnteranceBackground[i]->setToggleState(ToggleState::eNormal);
	}

	if (SpriteMonster[0])
		SpriteMonster[0]->setVisible(false);
	if (SpriteMonster[1])
		SpriteMonster[1]->setVisible(true);

	LabelName->setVisible(true);
	LabelDensity->setVisible(true);
}

void UI_ChannelSelect::ChannelButton::selected()
{
	const EnteranceType_t entranceType = Info ? Info->enteranceType_ : EnteranceType::Normal;

	State_ = Selected;

	for (int i = 0; i < EnteranceType::Max; ++i)
	{
		TBtnEnteranceBackground[i]->setVisible(i == entranceType);
		TBtnEnteranceBackground[i]->setToggleState(ToggleState::eToggled);
	}

	if (SpriteMonster[0])
		SpriteMonster[0]->setVisible(true);
	if (SpriteMonster[1])
		SpriteMonster[1]->setVisible(false);
	LabelName->setVisible(true);
	LabelDensity->setVisible(true);
}

void UI_ChannelSelect::ChannelButton::invisible()
{
	State_ = Invisible;

	for (int i = 0; i < EnteranceType::Max; ++i)
	{
		TBtnEnteranceBackground[i]->setVisible(false);
	}

	if (SpriteMonster[0])
		SpriteMonster[0]->setVisible(false);
	if (SpriteMonster[1])
		SpriteMonster[1]->setVisible(false);
	LabelName->setVisible(false);
	LabelDensity->setVisible(false);
}

void UI_ChannelSelect::ChannelButton::mouseOver()
{
	SpriteMouseOverBorder->setVisible(true);
}

void UI_ChannelSelect::ChannelButton::mouseLeave()
{
	SpriteMouseOverBorder->setVisible(false);
}

void UI_ChannelSelect::ChannelButton::initMonsterSprites(UI_ChannelSelect* masterGroup)
{
	if (Info == nullptr)
	{
		_LogWarn_("ChannelButton::initMonsterSprites");
	}

	UIGroup* pParent = Core::Contents.UIManager->getGroup(UI_CHANNEL_GROUP_CHANNEL_LIST);

	int iSelectedSprite = Info ? Info->SelectedSpriteIndex : InvalidValue_v;
	int iNormalSprite = Info ? Info->NormalSpriteIndex : InvalidValue_v;

	if (SpriteMonster[0] == nullptr)
	{
		UISpriteInfo* pSelectedInfo = dbg_new UISpriteInfo();
		pSelectedInfo->Sga = masterGroup->getMonsterSpriteSgaIndex();
		pSelectedInfo->Img = masterGroup->getMonsterSpriteImgIndex();
		pSelectedInfo->Sprite = iSelectedSprite;

		SpriteMonster[0] = UISprite::create(masterGroup, pParent, pSelectedInfo, true);
		SpriteMonster[0]->SetDeveloperCreated(true);
		SpriteMonster[0]->Load();
		SpriteMonster[0]->SetRelativePosition(TBtnEnteranceBackground[0], 5, 5, HAlignment::Left, VAlignment::Bottom);


		UISpriteInfo* pNormalInfo = dbg_new UISpriteInfo(*pSelectedInfo);
		pNormalInfo->Sprite = iNormalSprite;

		SpriteMonster[1] = UISprite::create(masterGroup, pParent, pNormalInfo, true);
		SpriteMonster[1]->SetDeveloperCreated(true);
		SpriteMonster[1]->Load();
		SpriteMonster[1]->SetRelativePosition(TBtnEnteranceBackground[0], 5, 5, HAlignment::Left, VAlignment::Bottom);


		pParent->addChild(SpriteMonster[0]);
		pParent->addChild(SpriteMonster[1]);
	}
	else
	{
		SpriteMonster[0]->getInfo()->Sprite = Info->SelectedSpriteIndex;
		SpriteMonster[0]->Reload();
		SpriteMonster[1]->getInfo()->Sprite = Info->NormalSpriteIndex;
		SpriteMonster[1]->Reload();
	}
}

void UI_ChannelSelect::ChannelButton::setChannelInfo(const LobbyChannelInfo& channelData, char sequence)
{
	Info = Core::DataManager->getChannelInfo(channelData.Type);
	const SGString& szFmtName = LabelName->getInfo()->Text; // szFmtName = "ch%02d.%s %c"
	LabelName->setText(StringUtils::format(szFmtName.Source(), channelData.Number, Info->name_.Source(), sequence));

	LabelDensity->setText(ChannelDensity::Name[channelData.Desity]);
	LabelDensity->setColor(ChannelDensityEx::Color[channelData.Desity]);
}

bool UI_ChannelSelect::ChannelButton::containsPoint(const SGVec2& pos)
{
	for (int i = 0; i < EnteranceType::Max; ++i)
	{
		if (TBtnEnteranceBackground[i]->isVisible() && TBtnEnteranceBackground[i]->GetWorldBoundingBox().
			containsPoint(pos))
		{
			return true;
		}
	}
	return false;
}

void UI_ChannelSelect::OnInit()
{
	monsterSpriteSgaIndex_ = Core::Contents.PackManager->getPackIndexDefault(SG_MONSTER_SPRITE_SGA);
	monsterSpriteImgIndex_ = Core::Contents.PackManager->getImgIndexUnsafe(
		monsterSpriteSgaIndex_, SG_MONSTER_SPRITE_IMG);

	if (monsterSpriteSgaIndex_ == InvalidValue_v)
	{
		_LogWarn_("채널 몬스터 SgaIndex 정보를 얻는데 실패했습니다.");
	}

	if (monsterSpriteImgIndex_ == InvalidValue_v)
	{
		_LogWarn_("채널 몬스터 ImngIndex 정보를 얻는데 실패했습니다.");
	}

	spriteBackgroundGear_ = Core::Contents.UIManager->getSprite(UI_CHANNEL_SPRITE_BACKGROUND_GEAR);
	btnStart_ = Core::Contents.UIManager->getButton(UI_CHANNEL_BUTTON_START);
	btnTerminate_ = Core::Contents.UIManager->getButton(UI_CHANNEL_BUTTON_TERMINATE);

	btnRefresh_ = Core::Contents.UIManager->getButton(UI_CHANNEL_CHANNEL_LIST_BUTTON_REFRESH);
	toggleTeen_ = Core::Contents.UIManager->getToggleButton(UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_TEEN);
	toggleAdult_ = Core::Contents.UIManager->getToggleButton(UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_ADULT);
	labelPage_ = Core::Contents.UIManager->getLabel(UI_CHANNEL_CHANNEL_LIST_LABEL_PAGE);

	for (int i = 0; i < GameServerType::Max; ++i)
	{
		serverButtons_[i] = dbg_new ServerButton{ (GameServerType_t)i };
	}

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		channelButtons_[i] = dbg_new ChannelButton(i);
	}
}

void UI_ChannelSelect::OnLoaded()
{
	selectChannelTab(ChannelTab::Teen);

	SGSprite* pGearSprite = spriteBackgroundGear_->source();
	pGearSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	pGearSprite->setPosition(Core::ClientInfo->uiResolutionSize_.width / 2.0f,
	                         Core::ClientInfo->uiResolutionSize_.height / 2.0f);

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = channelButtons_[i];
		pButton->setState(ChannelButton::Normal);
	}

	LobbyChannelInfo a[4]{
		{ 1, 1, ChannelDensity::Free },
		{ 2, 11, ChannelDensity::Normal },
		{ 3, 13, ChannelDensity::Busy },
		{ 4, 14, ChannelDensity::Full },
	};

	channelButtons_[0]->setChannelInfo(a[0], 'A');
	channelButtons_[0]->initMonsterSprites(this);
	channelButtons_[0]->setState(ChannelButton::Selected);

	channelButtons_[1]->setChannelInfo(a[1], 'A');
	channelButtons_[1]->initMonsterSprites(this);
	channelButtons_[1]->setState(ChannelButton::Normal);

	channelButtons_[2]->setChannelInfo(a[2], 'A');
	channelButtons_[2]->initMonsterSprites(this);
	channelButtons_[2]->setState(ChannelButton::Normal);

	channelButtons_[3]->setChannelInfo(a[3], 'A');
	channelButtons_[3]->initMonsterSprites(this);
	channelButtons_[3]->setState(ChannelButton::Normal);
}

void UI_ChannelSelect::OnUnloaded()
{
	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = channelButtons_[i];
		pButton->SpriteMonster[0] = nullptr;
		pButton->SpriteMonster[1] = nullptr;
	}
}

void UI_ChannelSelect::onUpdate(float _dt)
{
	updateServerSelectionSpriteOpacity(_dt);
	updateBackgroundGearRotation(_dt);
}

void UI_ChannelSelect::OnMouseDown(SGEventMouse* _pMouseEvent)
{
	const SGVec2 cursorPos = _pMouseEvent->getCursorPos();

	for (int i = 0; i < GameServerType::Max; ++i)
	{
		ServerButton* pButton = serverButtons_[i];
		if (pButton->State_ == ServerButton::Normal && pButton->containsPoint(cursorPos))
		{
			selectServer(GameServerType_t(i));
			break;
		}
	}

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = channelButtons_[i];
		if (pButton->State_ == ChannelButton::Normal && pButton->containsPoint(cursorPos))
		{
			selectChannel(i);
			break;
		}
	}
}

void UI_ChannelSelect::OnMouseMove(SGEventMouse* _pMouseEvent)
{
	const SGVec2 cursorPos = _pMouseEvent->getCursorPos();

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = channelButtons_[i];

		if (pButton->State_ == ChannelButton::Invisible)
			continue;

		if (pButton->containsPoint(cursorPos))
		{
			pButton->mouseOver();
		}
		else
		{
			pButton->mouseLeave();
		}
	}
}

void UI_ChannelSelect::OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state)
{
	JCORE_UNUSED(_state);

	switch (_pToggleBtn->GetCode())
	{
	case UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_TEEN:
		selectChannelTab(ChannelTab::Teen);
		break;
	case UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_ADULT:
		selectChannelTab(ChannelTab::Adult);
		break;
	default:
		break;
	}
}

void UI_ChannelSelect::updateServerSelectionSpriteOpacity(float dt)
{
	if (selectedServer_ == GameServerType::Max)
	{
		return;
	}

	if (serverButtonSelectionOpacityUp_)
		serverButtonSelectionOpacity_ += dt * 0.3f;
	else
		serverButtonSelectionOpacity_ -= dt * 0.3f;

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

	serverButtons_[selectedServer_]->SpriteSelectedBackground->setOpacityF(serverButtonSelectionOpacity_);
}

void UI_ChannelSelect::updateBackgroundGearRotation(float dt)
{
	SGSprite* pGearSprite = spriteBackgroundGear_->source();
	pGearSprite->setRotation(pGearSprite->getRotation() + dt * SG_BACKGROUND_GEAR_SPEED);
}

void UI_ChannelSelect::selectServer(GameServerType_t _serverType)
{
	if (selectedServer_ == _serverType)
		return;

	selectedServer_ = _serverType;

	for (int i = 0; i < GameServerType::Max; ++i)
	{
		ServerButton* pButton = serverButtons_[i];
		if (i == _serverType)
			pButton->setState(ServerButton::Selected);
		else if (pButton->State_ == ServerButton::Selected)
			pButton->setState(ServerButton::Normal);
	}

	S_LOBBY::SEND_CLO_LoadChannelInfo(_serverType);
}

void UI_ChannelSelect::selectChannel(int _channelIndex)
{
	if (selectedChannelIndex_ == _channelIndex)
	{
		return;
	}

	selectedChannelIndex_ = _channelIndex;

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i)
	{
		ChannelButton* pButton = channelButtons_[i];
		if (i == _channelIndex)
			pButton->setState(ChannelButton::Selected);
		else if (pButton->State_ == ChannelButton::Selected)
			pButton->setState(ChannelButton::Normal);
	}
}

void UI_ChannelSelect::selectChannelTab(ChannelTab _tab)
{
	channelTab_ = _tab;

	if (channelTab_ == ChannelTab::Teen)
	{
		toggleAdult_->setToggleState(ToggleState::eNormal);
		toggleTeen_->setToggleState(ToggleState::eToggled);
	}
	else if (channelTab_ == ChannelTab::Adult)
	{
		toggleAdult_->setToggleState(ToggleState::eToggled);
		toggleTeen_->setToggleState(ToggleState::eNormal);
	}
}

void UI_ChannelSelect::enterChannel(GameServerType_t _serverType, int _channelIndex)
{
	JCORE_UNUSED(_serverType);
	JCORE_UNUSED(_channelIndex);
}
