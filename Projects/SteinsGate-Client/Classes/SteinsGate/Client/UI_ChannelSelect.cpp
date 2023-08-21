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

UI_ChannelSelect::UI_ChannelSelect(UIGroupInfo* groupInfo)
	: UIMasterGroup(groupInfo)
	, m_pSpriteBackgroundGear(nullptr)
	, m_pBtnStart{nullptr}
	, m_pBtnTerminate{nullptr}
	, m_pBtnRefresh{nullptr}
	, m_pTBtnTeen{nullptr}
	, m_pTBtnAdult{nullptr}
	, m_pLabelPage{nullptr}
	, m_arrServerButtonList{}
	, m_arrChannelButtonList{}
	, m_iMonsterSpriteSgaIndex(InvalidValue_v)
	, m_iMonsterSpriteImgIndex(InvalidValue_v)
	, m_fServerButtonSelectionOpacity(SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY)
	, m_bServerButtonSelectionOpacityUp(true)
	, m_eSelectedServer(GameServerType::Max)
	, m_iSelectedChannelIndex(InvalidValue_v)
	, m_eChannelTab(ChannelTab::Adult)
{}

UI_ChannelSelect::~UI_ChannelSelect() {
	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i) {
		JCORE_DELETE_SAFE(m_arrChannelButtonList[i]);
	}

	for (int i = 0; i < GameServerType::Max; ++i) {
		JCORE_DELETE_SAFE(m_arrServerButtonList[i]);
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
	, State_(Disabled) {
	initSprites(type);
	setState(Normal);
}

void UI_ChannelSelect::ServerButton::initSprites(GameServerType_t type) {
	Group = CoreUIManager_v->getGroup(UI_CHANNEL_SERVER_BUTTON_LIST_GROUP_LUKE + 1000 * type);
	SpriteServerCharacter = CoreUIManager_v->getSprite(UI_CHANNEL_SERVER_CHARACTER_LIST_SPRITE_LUKE + type);
	SpriteServerCharacter->setVisible(false);
	SpriteSelectedBackground = Group->getAtTemplated<UISprite>(0);
	SpriteSelectedBackground->setOpacity(SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY);
	SpriteSelectedText = Group->getAtTemplated<UISprite>(1);
	SpriteSelectedBorder = Group->getAtTemplated<UISprite>(2);
	SpriteNormalText = Group->getAtTemplated<UISprite>(3);
	SpriteNormalBorder = Group->getAtTemplated<UISprite>(4);
	SpriteDisabledText = Group->getAtTemplated<UISprite>(5);
}

void UI_ChannelSelect::ServerButton::setState(State state) {
	switch (state) {
	case Selected: select();	break;
	case Normal:   normal();	break;
	case Disabled: disabled();  break;
	}

}

void UI_ChannelSelect::ServerButton::select() {
	State_ = Selected;

	SpriteSelectedBackground->setVisible(true);
	SpriteSelectedText->setVisible(true);
	SpriteSelectedBorder->setVisible(true);
	SpriteNormalText->setVisible(false);
	SpriteNormalBorder->setVisible(false);
	SpriteDisabledText->setVisible(false);
	SpriteServerCharacter->setVisible(true);
}

void UI_ChannelSelect::ServerButton::normal() {
	State_ = Normal;

	SpriteSelectedBackground->setVisible(false);
	SpriteSelectedText->setVisible(false);
	SpriteSelectedBorder->setVisible(false);
	SpriteNormalText->setVisible(true);
	SpriteNormalBorder->setVisible(true);
	SpriteDisabledText->setVisible(false);
	SpriteServerCharacter->setVisible(false);
}

void UI_ChannelSelect::ServerButton::disabled() {
	State_ = Disabled;

	SpriteSelectedBackground->setVisible(false);
	SpriteSelectedText->setVisible(false);
	SpriteSelectedBorder->setVisible(false);
	SpriteNormalText->setVisible(false);
	SpriteNormalBorder->setVisible(false);
	SpriteDisabledText->setVisible(true);
	SpriteServerCharacter->setVisible(false);
}

bool UI_ChannelSelect::ServerButton::containsPoint(const SGVec2& pos) {
	if (!SpriteNormalText->isVisible())
		return false;


	const SGRect box = SpriteNormalText->getWorldBoundingBox();

	if (box.containsPoint(pos)) {
		RectEx::log(box);
		Vec2Ex::log(pos);
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
	, SpriteMonster{nullptr}
	, LabelName(nullptr)
	, LabelDensity(nullptr)
	, State_(Invisible)
	, Info() {
	initSprites();
	invisible();
	mouseLeave();
}

void UI_ChannelSelect::ChannelButton::initSprites() {
	// 6 = SLOT_1_TYPE_0 ~ SLOT_2_TYPE_0 사이의 엘리먼트 갯수가 6개이므로
	TBtnEnteranceBackground[0] = CoreUIManager_v->getToggleButton(UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_SLOT_1_TYPE_0 + 6 * Index);
	TBtnEnteranceBackground[0]->setInternalDetailEventEnabled(false);
	TBtnEnteranceBackground[1] = CoreUIManager_v->getToggleButton(UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_SLOT_1_TYPE_1 + 6 * Index);
	TBtnEnteranceBackground[1]->setInternalDetailEventEnabled(false);
	TBtnEnteranceBackground[2] = CoreUIManager_v->getToggleButton(UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_SLOT_1_TYPE_2 + 6 * Index);
	TBtnEnteranceBackground[2]->setInternalDetailEventEnabled(false);

	SpriteMouseOverBorder = CoreUIManager_v->getSprite(UI_CHANNEL_CHANNEL_LIST_SPRITE_SLOT_1_OVER + 6 * Index);
	LabelName = CoreUIManager_v->getLabel(UI_CHANNEL_CHANNEL_LIST_LABEL_SLOT_1_NAME + 6 * Index);
	LabelDensity = CoreUIManager_v->getLabel(UI_CHANNEL_CHANNEL_LIST_LABEL_SLOT_1_DENSITY + 6 * Index);
	LabelDensity->source()->enableOutline(Color4B::BLACK, 3);
}

void UI_ChannelSelect::ChannelButton::setState(State state) {
	switch (state) {
	case Normal:	normal();	 break;
	case Selected:	selected();	 break;
	case Invisible: invisible(); break;
	}
}

void UI_ChannelSelect::ChannelButton::normal() {
	const EnteranceType_t eEnterence = Info ? Info->EnteranceType : EnteranceType::Normal;

	State_ = Normal;

	for (int i = 0; i < EnteranceType::Max; ++i) {
		TBtnEnteranceBackground[i]->setVisible(i == eEnterence);
		TBtnEnteranceBackground[i]->setToggleState(ToggleState::eNormal);
	}

	if (SpriteMonster[0])
		SpriteMonster[0]->setVisible(false);
	if (SpriteMonster[1])
		SpriteMonster[1]->setVisible(true);

	LabelName->setVisible(true);
	LabelDensity->setVisible(true);
}

void UI_ChannelSelect::ChannelButton::selected() {
	const EnteranceType_t eEnterence = Info ? Info->EnteranceType : EnteranceType::Normal;

	State_ = Selected;

	for (int i = 0; i < EnteranceType::Max; ++i) {
		TBtnEnteranceBackground[i]->setVisible(i == eEnterence);
		TBtnEnteranceBackground[i]->setToggleState(ToggleState::eToggled);
	}

	if (SpriteMonster[0])
		SpriteMonster[0]->setVisible(true);
	if (SpriteMonster[1])
		SpriteMonster[1]->setVisible(false);
	LabelName->setVisible(true);
	LabelDensity->setVisible(true);
}

void UI_ChannelSelect::ChannelButton::invisible() {
	State_ = Invisible;

	for (int i = 0; i < EnteranceType::Max; ++i) {
		TBtnEnteranceBackground[i]->setVisible(false);
	}

	if (SpriteMonster[0])
		SpriteMonster[0]->setVisible(false);
	if (SpriteMonster[1])
		SpriteMonster[1]->setVisible(false);
	LabelName->setVisible(false);
	LabelDensity->setVisible(false);
}

void UI_ChannelSelect::ChannelButton::mouseOver() {
	SpriteMouseOverBorder->setVisible(true);
}

void UI_ChannelSelect::ChannelButton::mouseLeave() {
	SpriteMouseOverBorder->setVisible(false);
}

void UI_ChannelSelect::ChannelButton::initMonsterSprites(UI_ChannelSelect* masterGroup) {
	if (Info == nullptr) {
		_LogWarn_("ChannelButton::initMonsterSprites");
	}

	UIGroup* pParent = CoreUIManager_v->getGroup(UI_CHANNEL_GROUP_CHANNEL_LIST);

	int iSelectedSprite = Info ? Info->SelectedSpriteIndex : InvalidValue_v;
	int iNormalSprite = Info ? Info->NormalSpriteIndex : InvalidValue_v;

	if (SpriteMonster[0] == nullptr) {
		UISpriteInfo* pSelectedInfo = dbg_new UISpriteInfo();
		pSelectedInfo->Sga = masterGroup->m_iMonsterSpriteSgaIndex;
		pSelectedInfo->Img = masterGroup->m_iMonsterSpriteImgIndex;
		pSelectedInfo->Sprite = iSelectedSprite;

		SpriteMonster[0] = UISprite::create(masterGroup, pParent, pSelectedInfo, true);
		SpriteMonster[0]->setDeveloperCreated(true);
		SpriteMonster[0]->load();
		SpriteMonster[0]->setRelativePosition(TBtnEnteranceBackground[0], 5, 5, HAlignment::Left, VAlignment::Bottom);
		

		UISpriteInfo* pNormalInfo = dbg_new UISpriteInfo(*pSelectedInfo);
		pNormalInfo->Sprite = iNormalSprite;

		SpriteMonster[1] = UISprite::create(masterGroup, pParent, pNormalInfo, true);
		SpriteMonster[1]->setDeveloperCreated(true);
		SpriteMonster[1]->load();
		SpriteMonster[1]->setRelativePosition(TBtnEnteranceBackground[0], 5, 5, HAlignment::Left, VAlignment::Bottom);
		

		pParent->addChild(SpriteMonster[0]);
		pParent->addChild(SpriteMonster[1]);
	} else {
		SpriteMonster[0]->getInfo()->Sprite = Info->SelectedSpriteIndex;
		SpriteMonster[0]->reload();
		SpriteMonster[1]->getInfo()->Sprite = Info->NormalSpriteIndex;
		SpriteMonster[1]->reload();
	}
}

void UI_ChannelSelect::ChannelButton::setChannelInfo(const LobbyChannelInfo& channelData, char sequence) {
	Info = CoreDataManager_v->getChannelInfo(channelData.Type);
	const SGString& szFmtName = LabelName->getInfo()->Text;	// szFmtName = "ch%02d.%s %c"
	LabelName->setText(StringUtils::format(szFmtName.Source(), channelData.Number, Info->Name.Source(), sequence));

	LabelDensity->setText(ChannelDensity::Name[channelData.Desity]);
	LabelDensity->setColor(ChannelDensityEx::Color[channelData.Desity]);
}

bool UI_ChannelSelect::ChannelButton::containsPoint(const SGVec2& pos) {
	for (int i = 0; i < EnteranceType::Max; ++i) {
		if (TBtnEnteranceBackground[i]->isVisible() && TBtnEnteranceBackground[i]->getWorldBoundingBox().containsPoint(pos)) {
			return true;
		}
	}
	return false;
}

void UI_ChannelSelect::onInit() {

	m_iMonsterSpriteSgaIndex = CorePackManager_v->getPackIndexDefault(SG_MONSTER_SPRITE_SGA);
	m_iMonsterSpriteImgIndex = CorePackManager_v->getImgIndexUnsafe(m_iMonsterSpriteSgaIndex, SG_MONSTER_SPRITE_IMG);

	if (m_iMonsterSpriteSgaIndex == InvalidValue_v) {
		_LogWarn_("채널 몬스터 SgaIndex 정보를 얻는데 실패했습니다.");
	}

	if (m_iMonsterSpriteImgIndex == InvalidValue_v) {
		_LogWarn_("채널 몬스터 ImngIndex 정보를 얻는데 실패했습니다.");
	}

	m_pSpriteBackgroundGear = CoreUIManager_v->getSprite(UI_CHANNEL_SPRITE_BACKGROUND_GEAR);
	m_pBtnStart = CoreUIManager_v->getButton(UI_CHANNEL_BUTTON_START);
	m_pBtnTerminate = CoreUIManager_v->getButton(UI_CHANNEL_BUTTON_TERMINATE);

	m_pBtnRefresh = CoreUIManager_v->getButton(UI_CHANNEL_CHANNEL_LIST_BUTTON_REFRESH);
	m_pTBtnTeen = CoreUIManager_v->getToggleButton(UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_TEEN);
	m_pTBtnAdult = CoreUIManager_v->getToggleButton(UI_CHANNEL_CHANNEL_LIST_TOGGLEBUTTON_ADULT);
	m_pLabelPage = CoreUIManager_v->getLabel(UI_CHANNEL_CHANNEL_LIST_LABEL_PAGE);


	for (int i = 0; i < GameServerType::Max; ++i) {
		m_arrServerButtonList[i] = dbg_new ServerButton{ (GameServerType_t)i };
	}

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i) {
		m_arrChannelButtonList[i] = dbg_new ChannelButton(i);
	}

	
}

void UI_ChannelSelect::onLoaded() {
	selectServer(GameServerType::Luke);
	selectChannelTab(ChannelTab::Teen);

	SGSprite* pGearSprite = m_pSpriteBackgroundGear->source();
	pGearSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	pGearSprite->setPosition(CoreClientInfo_v->UIResolutionSize.width / 2.0f, CoreClientInfo_v->UIResolutionSize.height / 2.0f);

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i) {
		ChannelButton* pButton = m_arrChannelButtonList[i];
		pButton->setState(ChannelButton::Normal);
	}

	LobbyChannelInfo a[4]{
		 {1, 1, ChannelDensity::Free },
		  {2, 11, ChannelDensity::Normal },
		 {3, 13, ChannelDensity::Busy },
		 {4, 14, ChannelDensity::Full },
	};

	m_arrChannelButtonList[0]->setChannelInfo(a[0], 'A');
	m_arrChannelButtonList[0]->initMonsterSprites(this);
	m_arrChannelButtonList[0]->setState(ChannelButton::Selected);

	m_arrChannelButtonList[1]->setChannelInfo(a[1], 'A');
	m_arrChannelButtonList[1]->initMonsterSprites(this);
	m_arrChannelButtonList[1]->setState(ChannelButton::Normal);

	m_arrChannelButtonList[2]->setChannelInfo(a[2], 'A');
	m_arrChannelButtonList[2]->initMonsterSprites(this);
	m_arrChannelButtonList[2]->setState(ChannelButton::Normal);

	m_arrChannelButtonList[3]->setChannelInfo(a[3], 'A');
	m_arrChannelButtonList[3]->initMonsterSprites(this);
	m_arrChannelButtonList[3]->setState(ChannelButton::Normal);
}

void UI_ChannelSelect::onUnloaded() {
	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i) {
		ChannelButton* pButton = m_arrChannelButtonList[i];
		pButton->SpriteMonster[0] = nullptr;
		pButton->SpriteMonster[1] = nullptr;
	}
}

void UI_ChannelSelect::onUpdate(float dt) {
	updateServerSelectionSpriteOpacity(dt);
	updateBackgroundGearRotation(dt);
}

void UI_ChannelSelect::onMouseDown(SGEventMouse* mouseEvent) {

	const SGVec2 cursorPos = mouseEvent->getCursorPos();

	for (int i = 0; i < GameServerType::Max; ++i) {
		ServerButton* pButton = m_arrServerButtonList[i];
		if (pButton->State_ == ServerButton::Normal && pButton->containsPoint(cursorPos)) {
			selectServer(GameServerType_t(i));
			break;
		}
	}

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i) {
		ChannelButton* pButton = m_arrChannelButtonList[i];
		if (pButton->State_ == ChannelButton::Normal && pButton->containsPoint(cursorPos)) {
			selectChannel(i);
			break;
		}
	}
}

void UI_ChannelSelect::onMouseMove(SGEventMouse* mouseEvent) {
	const SGVec2 cursorPos = mouseEvent->getCursorPos();

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i) {
		ChannelButton* pButton = m_arrChannelButtonList[i];

		if (pButton->State_ == ChannelButton::Invisible) 
			continue;

		if (pButton->containsPoint(cursorPos)) {
			pButton->mouseOver();
		} else {
			pButton->mouseLeave();
		}
	}
}

void UI_ChannelSelect::onToggleStateChanged(UIToggleButton* toggleBtn, ToggleState state) {
	switch (toggleBtn->getCode()) {
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

void UI_ChannelSelect::updateServerSelectionSpriteOpacity(float dt) {
	if (m_eSelectedServer == GameServerType::Max) {
		return;
	}

	if (m_bServerButtonSelectionOpacityUp)
		m_fServerButtonSelectionOpacity += dt * 0.3f;
	else
		m_fServerButtonSelectionOpacity -= dt * 0.3f;

	if (m_fServerButtonSelectionOpacity > SG_SERVER_BUTTON_SELECTION_SPRITE_MAX_OPACITY) {
		m_fServerButtonSelectionOpacity = SG_SERVER_BUTTON_SELECTION_SPRITE_MAX_OPACITY;
		m_bServerButtonSelectionOpacityUp = false;
	} else if (m_fServerButtonSelectionOpacity < SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY){
		m_fServerButtonSelectionOpacity = SG_SERVER_BUTTON_SELECTION_SPRITE_MIN_OPACITY;
		m_bServerButtonSelectionOpacityUp = true;
	}

	m_arrServerButtonList[m_eSelectedServer]->SpriteSelectedBackground->setOpacityF(m_fServerButtonSelectionOpacity);
}

void UI_ChannelSelect::updateBackgroundGearRotation(float dt) {
	SGSprite* pGearSprite = m_pSpriteBackgroundGear->source();
	pGearSprite->setRotation(pGearSprite->getRotation() + dt * SG_BACKGROUND_GEAR_SPEED);
}

void UI_ChannelSelect::selectServer(GameServerType_t serverType) {
	if (m_eSelectedServer == serverType) 
		return;

	m_eSelectedServer = serverType;

	for (int i = 0; i < GameServerType::Max; ++i) {
		ServerButton* pButton = m_arrServerButtonList[i];
		if (i == serverType) 
			pButton->setState(ServerButton::Selected);
		else if (pButton->State_ == ServerButton::Selected)
			pButton->setState(ServerButton::Normal);
	}
}

void UI_ChannelSelect::selectChannel(int channelIndex) {
	if (m_iSelectedChannelIndex == channelIndex) {
		return;
	}

	m_iSelectedChannelIndex = channelIndex;

	for (int i = 0; i < SG_UI_CHANNELSELECT_MAX_CHANNEL_COUNT; ++i) {
		ChannelButton* pButton = m_arrChannelButtonList[i];
		if (i == channelIndex) 
			pButton->setState(ChannelButton::Selected);
		else if (pButton->State_ == ChannelButton::Selected) 
			pButton->setState(ChannelButton::Normal);
	}
}

void UI_ChannelSelect::selectChannelTab(ChannelTab tab) {
	m_eChannelTab = tab;

	if (m_eChannelTab == ChannelTab::Teen) {
		m_pTBtnAdult->setToggleState(ToggleState::eNormal);
		m_pTBtnTeen->setToggleState(ToggleState::eToggled);
	} else if (m_eChannelTab == ChannelTab::Adult) {
		m_pTBtnAdult->setToggleState(ToggleState::eToggled);
		m_pTBtnTeen->setToggleState(ToggleState::eNormal);
	}
}

void UI_ChannelSelect::enterChannel(GameServerType_t serverType, int channelIndex) {

}

