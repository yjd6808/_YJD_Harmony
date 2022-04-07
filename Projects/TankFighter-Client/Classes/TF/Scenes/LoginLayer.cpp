#include <TF/Scenes/LoginLayer.h>
#include <TF/Object/Obstacle.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/UI/ColoredListView.h>
#include <TF/Network/GameClient.h>

#include "TF/Network/ClientConfiguration.h"

#define EDITBOX_ID				16
#define EDITBOX_PW				17

using namespace JNetwork;


bool LoginLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	Text* pLogoText = Text::create("탱크 파이터!", FONT_PATH_DEFAULT, 72);
	pLogoText->setSkewX(30.0f);
	pLogoText->setPosition({ 500, 500 });
	this->addChild(pLogoText);

	Text* pInfoText = Text::create("개발자 : 윤정도", FONT_PATH_DEFAULT, 25);
	pInfoText->enableGlow(Color4B::GRAY);
	pInfoText->setColor(ColorList::AbsoluteZero_v);
	pInfoText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	pInfoText->setSkewX(30.0f);
	pInfoText->setPosition({ 500, 400 });
	this->addChild(pInfoText);

	
	// 탱크 바디 - 바퀴 달린 부분
	m_pTank = Tank::create();
	this->addChild(m_pTank);

	m_pObstacle = Obstacle::create(ObstacleShape::Rhombus, 100, 100, 20);
	m_pObstacle->setPosition(150, 150);
	this->addChild(m_pObstacle);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(LoginLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(LoginLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	TextButton* loginBtn = TextButton::create(200, 45, "로그인", 15);
	loginBtn->setAnchorPoint(Vec2::ZERO);
	loginBtn->setPosition({ 400, 200 });
	loginBtn->setBackgroundColor(ColorList::Camel_v);
	loginBtn->setFontColor(Color3B::BLACK);
	loginBtn->setClickEvent(CC_CALLBACK_1(LoginLayer::OnClickedLoginButton, this));
	this->addChild(loginBtn);
	Button::create();

	TextButton* registerBtn = TextButton::create(200, 45, "회원가입", 15);
	registerBtn->setAnchorPoint(Vec2::ZERO);
	registerBtn->setPosition({ 400, 150 });
	registerBtn->setBackgroundColor(ColorList::Camel_v);
	registerBtn->setFontColor(Color3B::BLACK);
	registerBtn->setClickEvent(CC_CALLBACK_1(LoginLayer::OnClickedRegisterButton, this));
	this->addChild(registerBtn);

	TextButton* reconnectBtn = TextButton::create(200, 45, "서버 재접속 시도", 15);
	reconnectBtn->setAnchorPoint(Vec2::ZERO);
	reconnectBtn->setPosition({ 400, 100 });
	reconnectBtn->setBackgroundColor(ColorList::Camel_v);
	reconnectBtn->setFontColor(Color3B::BLACK);
	reconnectBtn->setClickEvent(CC_CALLBACK_1(LoginLayer::OnClickedReconnectButton, this));
	this->addChild(reconnectBtn);

	EditBox* pIDEditBox = EditBox::create(Size(200, 45), Scale9Sprite::create(RECT_IMG_FILENAME));
	pIDEditBox->setPosition({ 400, 300 });
	pIDEditBox->setFontColor(Color4B::WHITE);
	pIDEditBox->setColor(ColorList::Africanviolet_v);
	pIDEditBox->setFontSize(18.0f);
	pIDEditBox->setPlaceHolder("ID");
	pIDEditBox->setAnchorPoint(Vec2::ZERO);
	pIDEditBox->setPlaceholderFontColor(Color4B::GRAY);
	pIDEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	this->addChild(pIDEditBox, 0, EDITBOX_ID);
	pIDEditBox->setText("wjdeh515");
	
	EditBox* pPasswordEditBox = EditBox::create(Size(200, 45), Scale9Sprite::create(RECT_IMG_FILENAME));
	pPasswordEditBox->setPosition(Vec2(400, 250));
	pPasswordEditBox->setFontColor(Color4B::BLACK);
	pPasswordEditBox->setFontSize(18.0f);
	pPasswordEditBox->setColor(ColorList::Etonblue_v);
	pPasswordEditBox->setPlaceHolder("PASS");
	pPasswordEditBox->setAnchorPoint(Vec2::ZERO);
	pPasswordEditBox->setPlaceholderFontColor(Color4B::GRAY);
	pPasswordEditBox->setInputFlag(EditBox::InputFlag::PASSWORD);
	pPasswordEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	this->addChild(pPasswordEditBox, 0, EDITBOX_PW);
	pPasswordEditBox->setText("wjdeh414");
/*
	ColoredListView* pListView= ColoredListView::create(ColorUtil::To4B(ColorList::AoEnglish_v));
	pListView->SetContantSize({ 300, 300 });
	pListView->setPosition(300, 300);
	this->addChild(pListView);
	*/

/*
	TextScrollViewer* pView = TextScrollViewer::create();
	pView->setPosition(150, 150);
	pView->setColor(ColorList::Amethyst_v);
	pView->setContentSize({ 300, 300 });
	this->addChild(pView);
	*/

	/*
	LayerColor* l = LayerColor::create(Color4B::YELLOW);
	l->setContentSize({ 300, 300 });
	l->setPosition(300, 100);
	this->addChild(l);

	ListView* m_pLabelListView = ListView::create();
	m_pLabelListView->setDirection(ScrollView::Direction::VERTICAL);
	m_pLabelListView->setContentSize({300, 300});
	m_pLabelListView->setScrollBarEnabled(true);
	m_pLabelListView->setScrollBarAutoHideEnabled(false);
	m_pLabelListView->setAnchorPoint(Vec2::ZERO);
	//m_pLabelListView->addEventListener(CC_CALLBACK_2(TextScrollViewer::OnScrollViewEvent, this));
	l->addChild(m_pLabelListView);

	
	*/
	this->scheduleUpdate();
	return true;
}

bool IsAvailableIDPW(JCore::String& id)
{
	for (int i = 0; i < id.Length(); i++) {
		char& s = id[i];

		if (s >= 'A' && s <= 'Z' ||
			s >= 'a' && s <= 'z' ||
			s >= '0' && s <= '9')
			continue;

		return false;
	}

	return true;
}

void LoginLayer::OnClickedLoginButton(TextButton* sender) {
	EditBox* pIDEditBox = (EditBox*)this->getChildByTag(EDITBOX_ID);
	EditBox* pPWEditBox = (EditBox*)this->getChildByTag(EDITBOX_PW);

	JCore::String id = pIDEditBox->getText();
	JCore::String pw = pPWEditBox->getText();

	if (id.Length() > ID_LEN || pw.Length() > PASS_LEN) {
		PopUp::createInParent("아이디 비밀번호는 영문 + 숫자만 가능", this, false);
		return;
	}

	if (IsAvailableIDPW(id) && IsAvailableIDPW(pw)) {

		auto pPacket = new Packet<LoginSyn>();
		LoginSyn* pCmdLoginSyn = pPacket->Get<0>();
		strcpy_s(pCmdLoginSyn->Id, ID_LEN, id.Source());
		strcpy_s(pCmdLoginSyn->Password, PASS_LEN, pw.Source());

		if (_Client->SendAsync(pPacket) == false) {
			PopUp::createInParent("로그인 패킷 전송 실패", this, false);
		}
		return;
	}

	PopUp::createInParent("아이디 비밀번호를 똑바로 입력해주세요.", this, false);
}

void LoginLayer::OnClickedRegisterButton(TextButton* sender) {

	EditBox* pIDEditBox = (EditBox*)this->getChildByTag(EDITBOX_ID);
	EditBox* pPWEditBox = (EditBox*)this->getChildByTag(EDITBOX_PW);

	JCore::String id = pIDEditBox->getText();
	JCore::String pw = pPWEditBox->getText();

	if (id.Length() > ID_LEN || pw.Length() > PASS_LEN) {
		return;
	}

	if (IsAvailableIDPW(id) && IsAvailableIDPW(pw)) {

		auto pPacket = new Packet<RegisterSyn>();
		RegisterSyn* pCmdRegisterSyn = pPacket->Get<0>();
		strcpy_s(pCmdRegisterSyn->Id, ID_LEN, id.Source());
		strcpy_s(pCmdRegisterSyn->Password, PASS_LEN, pw.Source());

		if (_Client->SendAsync(pPacket) == false) {
			PopUp::createInParent("회원가입 패킷 전송 실패", this, false);
		}
		return;
	}

	PopUp::createInParent("아이디 비밀번호를 똑바로 입력해주세요.", this, false);
}


void LoginLayer::OnClickedReconnectButton(TextButton* sender) {
	if (_Client->ConnectAsync(JCore::StringUtil::Format("%s:%d", SERVER_ADDR, SERVER_PORT)) == false) {
		PopUp::createInParent("재접속에 실패하였습니다.", this, false);
	}
}


void LoginLayer::update(float delta) {
	m_pTank->updatePosition(delta);

	if (m_pObstacle->IsCollide(m_pTank)) {
		m_pTank->setPosition(m_pTank->GetPrevPos());
	}

	m_pTank->updateRotation(delta);
	if (m_pObstacle->IsCollide(m_pTank)) {
		m_pTank->setRotation(m_pTank->GetPrevRotation());
	}
}

void LoginLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
}

void LoginLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
}


/* =================================================================================
 *
 *                             통신 패킷 처리 
 *	
 * ================================================================================*/

void LoginLayer::CmdLoginAck(ICommand* cmd) {
	LoginAck* pLoginAck = cmd->CastCommand<LoginAck*>();

	// 로그인 성공
	if (pLoginAck->Result) {
		_Client->SetAccountUID(pLoginAck->UID);

		this->unscheduleUpdate();
		this->_eventDispatcher->removeAllEventListeners();
		Director::getInstance()->replaceScene(ChannelScene::createScene());
		return;
	}

	PopUp::createInParent(pLoginAck->Reason, this, false);

}

void LoginLayer::CmdRegisterAck(ICommand* cmd) {
	RegisterAck* pRegisterAck = cmd->CastCommand<RegisterAck*>();
	PopUp::createInParent(pRegisterAck->Reason, this, false);
}

