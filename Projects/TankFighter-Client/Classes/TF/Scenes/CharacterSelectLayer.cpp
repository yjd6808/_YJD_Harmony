#include <TF/Scenes/CharacterSelectLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>

#include <JCore/Ascii.h>

bool CharacterSelectLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	// 캐릭터 선택씬에 진입하면 캐릭터 정보를 받기위해 바로 패킷을 보낸다.
	SendFn::SendLoadCharacterInfoSyn();

	m_pCreateCharacterButton = TextButton::create(200, 45, "캐릭터 생성", 16);
	m_pCreateCharacterButton->setPosition({ 550, 300 });
	m_pCreateCharacterButton->setAnchorPoint(Vec2::ZERO);
	m_pCreateCharacterButton->setColor(ColorList::Babypink_v);
	m_pCreateCharacterButton->setFontColor(ColorList::Black_v);
	m_pCreateCharacterButton->setClickEvent(CC_CALLBACK_1(CharacterSelectLayer::OnClickedCreateCharacterButton, this));
	this->addChild(m_pCreateCharacterButton);

	m_pDeleteCharacterButton = TextButton::create(200, 45, "캐릭터 삭제", 16);
	m_pDeleteCharacterButton->setPosition({ 550, 250 });
	m_pDeleteCharacterButton->setAnchorPoint(Vec2::ZERO);
	m_pDeleteCharacterButton->setColor(ColorList::Babypink_v);
	m_pDeleteCharacterButton->setFontColor(ColorList::Black_v);
	m_pDeleteCharacterButton->setClickEvent(CC_CALLBACK_1(CharacterSelectLayer::OnClickedDeleteCharacterButton, this));
	this->addChild(m_pDeleteCharacterButton);

	m_pCharacterNickNameEditBox = EditBox::create(Size(200, 45), Scale9Sprite::create(RECT_IMG_FILENAME));
	m_pCharacterNickNameEditBox->setPosition({ 550, 350 });
	m_pCharacterNickNameEditBox->setFontColor(Color4B::WHITE);
	m_pCharacterNickNameEditBox->setColor(ColorList::Africanviolet_v);
	m_pCharacterNickNameEditBox->setFontSize(15);
	m_pCharacterNickNameEditBox->setPlaceHolder("삭제 또는 생성할 캐릭터명 입력");
	m_pCharacterNickNameEditBox->setPlaceholderFontSize(15);
	m_pCharacterNickNameEditBox->setAnchorPoint(Vec2::ZERO);
	m_pCharacterNickNameEditBox->setPlaceholderFontColor(Color4B::GRAY);
	m_pCharacterNickNameEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	this->addChild(m_pCharacterNickNameEditBox);

	return true;
}

bool CharacterSelectLayer::IsValidNickName(std::string& nick) {
	for (int i = 0; i < nick.size(); i++) {
		if (nick[i] == ' ') {
			return false;
		}
	}

	return true;
}

void CharacterSelectLayer::OnClickedCharacterButton(TextButton* btn) {
	// 버튼의 태그에 캐릭터 CharacterUID 정보가 들어있으므로
	SendFn::SendSelectCharacterSyn(btn->getTag());
}


void CharacterSelectLayer::OnClickedCreateCharacterButton(TextButton* sender) {

	std::string nickName = m_pCharacterNickNameEditBox->getText();

	if (nickName.length() == 0) {
		PopUp::createInParent("생성할 캐릭터 명을 입력해주세요.", this, false);
		return;
	}

	if (!IsValidNickName(nickName)) {
		PopUp::createInParent("공백이 포함되어 있습니다", this, false);
		return;
	}

	if (nickName.length() + 1 >= NAME_LEN) {
		PopUp::createInParent("닉네임은 40바이트 이하로 입력해주세요", this, false);
		return;
	}

	SendFn::SendCreateCharacterSyn(nickName);
}


void CharacterSelectLayer::OnClickedDeleteCharacterButton(TextButton* sender) {

	std::string nickName = m_pCharacterNickNameEditBox->getText();

	if (nickName.length() == 0) {
		PopUp::createInParent("생성할 캐릭터 명을 입력해주세요.", this, false);
		return;
	}

	if (!IsValidNickName(nickName)) {
		PopUp::createInParent("공백이 포함되어 있습니다", this, false);
		return;
	}

	if (nickName.length() + 1 >= NAME_LEN) {
		PopUp::createInParent("닉네임은 40바이트 이하로 입력해주세요", this, false);
		return;
	}

	SendFn::SendDeleteCharacterSyn(nickName);
}






/* =================================================================================
 *
 *                             통신 패킷 처리 
 *	
 * ================================================================================*/




void CharacterSelectLayer::CmdLoadCharacterInfoAck(ICommand* cmd) {

	// 캐릭터 정보가 갱신될 수 있으므로 먼저 다 비워준다.
	for (int i = 0; i < m_CharacterSelectButtonVec.Size(); i++) {
		m_CharacterSelectButtonVec[i]->getParent()->removeChild(m_CharacterSelectButtonVec[i]);
	}
	m_CharacterSelectButtonVec.Clear();

	LoadCharacterInfoAck* pLoadCharacterInfoAck = cmd->CastCommand<LoadCharacterInfoAck*>();

	if (pLoadCharacterInfoAck->Count == 0) {
		PopUp::createInParent("캐릭터가 없습니다. 생성해주세요.", this, false);
	}

	float iXpos = 100;
	float iYpos = 400;

	
	for (int i = 0, j = 0; i < pLoadCharacterInfoAck->Count; i++, j++) {
		CharacterInfo* info = &pLoadCharacterInfoAck->Info[i];

		// 2열 종대형 배치를 위해
		if (i != 0 && i % 2 == 0) {
			iYpos -= 50;
		}

		if (j == 0) {
			iXpos = 100;
		} else if (j == 1 ) {
			iXpos = 300;
			j = -1;
		}

		// 태그에 캐릭터 UID를 넣어줌
		TextButton* pCharacterBtn = TextButton::create(190, 45, StringUtils::format("%s\n%d Gold", info->Name, info->Money), 16);
		pCharacterBtn->setPosition({ iXpos, iYpos });
		pCharacterBtn->setAnchorPoint(Vec2::ZERO);
		pCharacterBtn->setClickEvent(CC_CALLBACK_1(CharacterSelectLayer::OnClickedCharacterButton, this));
		this->addChild(pCharacterBtn, 0, info->CharacterUID);
		m_CharacterSelectButtonVec.PushBack(pCharacterBtn);
	}
}

void CharacterSelectLayer::CmdSelectCharacterAck(ICommand* cmd) {
	SelectCharacterAck* pSelectCharacterAck = cmd->CastCommand<SelectCharacterAck*>();

	if (pSelectCharacterAck->Result) {
		// 캐릭터 CharacterUID 세팅
		_Client->SetCharacterUID(pSelectCharacterAck->CharacterUID);

		this->unscheduleUpdate();
		this->_eventDispatcher->removeAllEventListeners();
		Director::getInstance()->replaceScene(LobbyScene::createScene());
		return;
	}

	PopUp::createInParent(pSelectCharacterAck->Reason, this, false);
}

void CharacterSelectLayer::CmdCreateCharacterAck(ICommand* cmd) {
	CreateCharacterAck* pCreateCharacterAck = cmd->CastCommand<CreateCharacterAck*>();
	PopUp::createInParent(pCreateCharacterAck->Reason, this, false);
}

void CharacterSelectLayer::CmdDeleteCharacterAck(ICommand* cmd) {
	DeleteCharacterAck* pDeleteCharacterAck = cmd->CastCommand<DeleteCharacterAck*>();
	PopUp::createInParent(pDeleteCharacterAck->Reason, this, false);
}

