#include <TF/Scenes/CharacterSelectLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>

#include <JCore/Ascii.h>

bool CharacterSelectLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	// ĳ���� ���þ��� �����ϸ� ĳ���� ������ �ޱ����� �ٷ� ��Ŷ�� ������.
	SendFn::SendLoadCharacterInfoSyn();

	m_pCreateCharacterButton = TextButton::create(200, 45, "ĳ���� ����", 16);
	m_pCreateCharacterButton->setPosition({ 550, 300 });
	m_pCreateCharacterButton->setAnchorPoint(Vec2::ZERO);
	m_pCreateCharacterButton->setColor(ColorList::Babypink_v);
	m_pCreateCharacterButton->setFontColor(ColorList::Black_v);
	m_pCreateCharacterButton->setClickEvent(CC_CALLBACK_1(CharacterSelectLayer::OnClickedCreateCharacterButton, this));
	this->addChild(m_pCreateCharacterButton);

	m_pDeleteCharacterButton = TextButton::create(200, 45, "ĳ���� ����", 16);
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
	m_pCharacterNickNameEditBox->setPlaceHolder("���� �Ǵ� ������ ĳ���͸� �Է�");
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
	// ��ư�� �±׿� ĳ���� CharacterUID ������ ��������Ƿ�
	SendFn::SendSelectCharacterSyn(btn->getTag());
}


void CharacterSelectLayer::OnClickedCreateCharacterButton(TextButton* sender) {

	std::string nickName = m_pCharacterNickNameEditBox->getText();

	if (nickName.length() == 0) {
		PopUp::createInParent("������ ĳ���� ���� �Է����ּ���.", this, false);
		return;
	}

	if (!IsValidNickName(nickName)) {
		PopUp::createInParent("������ ���ԵǾ� �ֽ��ϴ�", this, false);
		return;
	}

	if (nickName.length() + 1 >= NAME_LEN) {
		PopUp::createInParent("�г����� 40����Ʈ ���Ϸ� �Է����ּ���", this, false);
		return;
	}

	SendFn::SendCreateCharacterSyn(nickName);
}


void CharacterSelectLayer::OnClickedDeleteCharacterButton(TextButton* sender) {

	std::string nickName = m_pCharacterNickNameEditBox->getText();

	if (nickName.length() == 0) {
		PopUp::createInParent("������ ĳ���� ���� �Է����ּ���.", this, false);
		return;
	}

	if (!IsValidNickName(nickName)) {
		PopUp::createInParent("������ ���ԵǾ� �ֽ��ϴ�", this, false);
		return;
	}

	if (nickName.length() + 1 >= NAME_LEN) {
		PopUp::createInParent("�г����� 40����Ʈ ���Ϸ� �Է����ּ���", this, false);
		return;
	}

	SendFn::SendDeleteCharacterSyn(nickName);
}






/* =================================================================================
 *
 *                             ��� ��Ŷ ó�� 
 *	
 * ================================================================================*/




void CharacterSelectLayer::CmdLoadCharacterInfoAck(ICommand* cmd) {

	// ĳ���� ������ ���ŵ� �� �����Ƿ� ���� �� ����ش�.
	for (int i = 0; i < m_CharacterSelectButtonVec.Size(); i++) {
		m_CharacterSelectButtonVec[i]->getParent()->removeChild(m_CharacterSelectButtonVec[i]);
	}
	m_CharacterSelectButtonVec.Clear();

	LoadCharacterInfoAck* pLoadCharacterInfoAck = cmd->CastCommand<LoadCharacterInfoAck*>();

	if (pLoadCharacterInfoAck->Count == 0) {
		PopUp::createInParent("ĳ���Ͱ� �����ϴ�. �������ּ���.", this, false);
	}

	float iXpos = 100;
	float iYpos = 400;

	
	for (int i = 0, j = 0; i < pLoadCharacterInfoAck->Count; i++, j++) {
		CharacterInfo* info = &pLoadCharacterInfoAck->Info[i];

		// 2�� ������ ��ġ�� ����
		if (i != 0 && i % 2 == 0) {
			iYpos -= 50;
		}

		if (j == 0) {
			iXpos = 100;
		} else if (j == 1 ) {
			iXpos = 300;
			j = -1;
		}

		// �±׿� ĳ���� UID�� �־���
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
		// ĳ���� CharacterUID ����
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

