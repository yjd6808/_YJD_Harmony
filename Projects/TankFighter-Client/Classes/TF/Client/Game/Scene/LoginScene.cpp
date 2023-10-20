/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:32:29 PM
 * =====================
 *
 */

#include "Pch.h"
#include "LoginScene.h"

#include <TF/Client/Net/Send/S_GAME.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_CCUI;

LoginScene::LoginScene()
	: m_pIdEditBox(nullptr)
	, m_pPassEditBox(nullptr)
{}

LoginScene::~LoginScene()
{}


bool LoginScene::init() {

	if (!BaseScene::init()) {
		return false;
	}

	Text* pLogoText = Text::create("탱크 파이터!", Const::Resource::FontName, 72);
	pLogoText->setSkewX(30.0f);
	pLogoText->setPosition({ 500, 500 });
	m_pUILayer->addChild(pLogoText);

	Text* pInfoText = Text::create("개발자 : 윤정도", Const::Resource::FontName, 25);
	pInfoText->enableGlow(Color4B::GRAY);
	pInfoText->setColor(ColorList::AbsoluteZero_v);
	pInfoText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	pInfoText->setPosition({ 500, 400 });
	m_pUILayer->addChild(pInfoText);


	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(LoginScene::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	TextButton* loginBtn = TextButton::create(200, 45, "로그인", 15);
	loginBtn->setAnchorPoint(Vec2::ZERO);
	loginBtn->setPosition({ 400, 200 });
	loginBtn->setBackgroundColor(ColorList::Camel_v);
	loginBtn->setFontColor(Color3B::BLACK);
	loginBtn->setClickEvent(CC_CALLBACK_1(LoginScene::OnClickedLoginButton, this));
	m_pUILayer->addChild(loginBtn);
	Button::create();

	TextButton* registerBtn = TextButton::create(200, 45, "회원가입", 15);
	registerBtn->setAnchorPoint(Vec2::ZERO);
	registerBtn->setPosition({ 400, 150 });
	registerBtn->setBackgroundColor(ColorList::Camel_v);
	registerBtn->setFontColor(Color3B::BLACK);
	registerBtn->setClickEvent(CC_CALLBACK_1(LoginScene::OnClickedRegisterButton, this));
	m_pUILayer->addChild(registerBtn);

	TextButton* reconnectBtn = TextButton::create(200, 45, "서버 재접속 시도", 15);
	reconnectBtn->setAnchorPoint(Vec2::ZERO);
	reconnectBtn->setPosition({ 400, 100 });
	reconnectBtn->setBackgroundColor(ColorList::Camel_v);
	reconnectBtn->setFontColor(Color3B::BLACK);
	reconnectBtn->setClickEvent(CC_CALLBACK_1(LoginScene::OnClickedReconnectButton, this));
	m_pUILayer->addChild(reconnectBtn);

	// 빠른 로그인을 위해 복붙
	HashMap<int, Tuple<std::string, std::string>> randIdPwMap{
		{1, { "wjdeh515", "wjdeh414" }},
		{2, { "wjdeh616", "wjdeh515" }},
		{3, { "wjdeh717", "wjdeh616" }},
		{4, { "wjdeh818", "wjdeh717" }},
		{5, { "wjdeh919", "wjdeh818" }},
		{6, { "wjdeh10110", "wjdeh919" }},
		{7 ,{ "wjdeh11111", "wjdeh10110" }},
	};

	const int idCode = Random().GenerateInt(1, 8);

	m_pIdEditBox = EditBox::create(Size(200, 45), Scale9Sprite::create(Const::Resource::WhiteRectFileName));
	m_pIdEditBox->addClickEventListener([](Ref* ref) { ((EditBox*)ref)->openKeyboard(); });
	m_pIdEditBox->setPosition({ 400, 300 });
	m_pIdEditBox->setFontColor(Color4B::WHITE);
	m_pIdEditBox->setColor(ColorList::Africanviolet_v);
	m_pIdEditBox->setFontSize(18.0f);
	m_pIdEditBox->setPlaceHolder("ID");
	m_pIdEditBox->setAnchorPoint(Vec2::ZERO);
	m_pIdEditBox->setPlaceholderFontColor(Color4B::GRAY);
	m_pIdEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	m_pIdEditBox->setText(randIdPwMap[idCode].item1.c_str());
	m_pUILayer->addChild(m_pIdEditBox, 0);
	
	m_pPassEditBox = EditBox::create(Size(200, 45), Scale9Sprite::create(Const::Resource::WhiteRectFileName));
	m_pPassEditBox->addClickEventListener([](Ref* ref) { ((EditBox*)ref)->openKeyboard(); });
	m_pPassEditBox->setPosition(Vec2(400, 250));
	m_pPassEditBox->setFontColor(Color4B::BLACK);
	m_pPassEditBox->setFontSize(18.0f);
	m_pPassEditBox->setColor(ColorList::Etonblue_v);
	m_pPassEditBox->setPlaceHolder("PASS");
	m_pPassEditBox->setAnchorPoint(Vec2::ZERO);
	m_pPassEditBox->setPlaceholderFontColor(Color4B::GRAY);
	m_pPassEditBox->setInputFlag(EditBox::InputFlag::PASSWORD);
	m_pPassEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	m_pPassEditBox->setText(randIdPwMap[idCode].item2.c_str());
	m_pUILayer->addChild(m_pPassEditBox, 0);

	return true;

}

void LoginScene::update(float delta) {
	BaseScene::update(delta);
}

void LoginScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
		OnClickedLoginButton(nullptr);
	}
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

void LoginScene::OnClickedLoginButton(TextButton* sender) {
	JCore::String id = m_pIdEditBox->getText();
	JCore::String pw = m_pPassEditBox->getText();

	if (id.Length() > Const::Length::Id || pw.Length() > Const::Length::Pass) {
		PopUp::createInParent("아이디 비밀번호는 영문 + 숫자만 가능", this, false);
		return;
	}

	if (IsAvailableIDPW(id) && IsAvailableIDPW(pw)) {
		S_GAME::SEND_CS_Login(id, pw);
		return;
	}

	PopUp::createInParent("아이디 비밀번호를 똑바로 입력해주세요.", this, false);
}

void LoginScene::OnClickedRegisterButton(TextButton* sender) {
	EditBox* pIDEditBox = m_pIdEditBox;
	EditBox* pPWEditBox = m_pPassEditBox;

	JCore::String id = pIDEditBox->getText();
	JCore::String pw = pPWEditBox->getText();

	if (id.Length() > Const::Length::Id || pw.Length() > Const::Length::Pass) {
		return;
	}

	if (IsAvailableIDPW(id) && IsAvailableIDPW(pw)) {
		S_GAME::SEND_CS_Register(id, pw);
		return;
	}

	PopUp::createInParent("아이디 비밀번호를 똑바로 입력해주세요.", this, false);
}


void LoginScene::OnClickedReconnectButton(TextButton* sender) {
	GameClient* pClient = Core::NetCore->getGameClient();
	if (pClient == nullptr) return;

	if (pClient->GetState() == JNetwork::Host::eConnected) {
		PopUp::createInParent("이미 게임서버와 연결되어 있습니다.", this, false);
		return;
	}

	Core::GameClient->SetReconnectMode(true);
	pClient->ConnectAsync(JNetwork::IPv4EndPoint::Parse(Const::Net::Server::GameConnectEndPoint));
}
