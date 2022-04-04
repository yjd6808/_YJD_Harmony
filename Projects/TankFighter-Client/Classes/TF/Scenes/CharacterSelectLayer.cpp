#include <TF/Scenes/CharacterSelectLayer.h>
#include <TF/Object/Obstacle.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/UI/ColoredListView.h>
#include <TF/Network/GameClient.h>
#include <TF/Network/SendFn.h>

#include <JCore/Ascii.h>

using namespace JNetwork;

bool CharacterSelectLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	// ĳ���� ���þ��� �����ϸ� ĳ���� ������ �ޱ����� �ٷ� ��Ŷ�� ������.
	SendFn::SendLoadCharacterInfoSyn();

	m_pCreateCharacterButton = TextButton::create(200, 45, "ĳ���� ����", 16);
	m_pCreateCharacterButton->setPosition(350, 200);
	m_pCreateCharacterButton->setColor(ColorList::Babypink_v);
	m_pCreateCharacterButton->setFontColor(ColorList::Black_v);
	m_pCreateCharacterButton->setClickEvent(CC_CALLBACK_1(CharacterSelectLayer::OnClickedCreateCharacterButton, this));
	this->addChild(m_pCreateCharacterButton);

	m_pCreateCharacterButton = TextButton::create(200, 45, "ĳ���� ����", 16);
	m_pCreateCharacterButton->setPosition(350, 150);
	m_pCreateCharacterButton->setColor(ColorList::Babypink_v);
	m_pCreateCharacterButton->setFontColor(ColorList::Black_v);
	m_pCreateCharacterButton->setClickEvent(CC_CALLBACK_1(CharacterSelectLayer::OnClickedCreateCharacterButton, this));

	m_pCharacterNickNameEditBox = EditBox::create(Size(200, 45), Scale9Sprite::create(RECT_IMG_FILENAME));
	m_pCharacterNickNameEditBox->setPosition({ 350, 250 });
	m_pCharacterNickNameEditBox->setFontColor(Color4B::WHITE);
	m_pCharacterNickNameEditBox->setColor(ColorList::Africanviolet_v);
	m_pCharacterNickNameEditBox->setFontSize(18.0f);
	m_pCharacterNickNameEditBox->setPlaceHolder("ID");
	m_pCharacterNickNameEditBox->setAnchorPoint(Vec2::ZERO);
	m_pCharacterNickNameEditBox->setPlaceholderFontColor(Color4B::GRAY);
	m_pCharacterNickNameEditBox->setPlaceHolder("���� �Ǵ� ������ ĳ���͸� �Է�");
	m_pCharacterNickNameEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	this->addChild(m_pCharacterNickNameEditBox);

	return true;
}

bool CharacterSelectLayer::IsValidNickName(std::string& nick) {
	for (int i = 0; i < nick.size(); i++) {
		if (JCore::IsSpace(nick[i])) {
			return false;
		}
	}

	return true;
}

void CharacterSelectLayer::OnClickedCreateCharacterButton(TextButton* sender) {

	std::string nickName = m_pCharacterNickNameEditBox->getText();

	if (IsValidNickName(nickName))
		PopUp::createInParent("������ ���ԵǾ� �ֽ��ϴ�", this, false);

	if (nickName.length() + 1 >= NAME_LEN) {
		PopUp::createInParent("�г����� 40����Ʈ ���Ϸ� �Է����ּ���", this, false);
		return;
	}

	SendFn::SendCreateCharacterSyn(nickName);
}


void CharacterSelectLayer::OnClickedDeleteCharacterButton(TextButton* sender) {

	std::string nickName = m_pCharacterNickNameEditBox->getText();

	if (IsValidNickName(nickName))
		PopUp::createInParent("������ ���ԵǾ� �ֽ��ϴ�", this, false);

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
	LoadCharacterInfoAck* pLoadCharacterInfoAck = cmd->CastCommand<LoadCharacterInfoAck*>();

	/*PopUp::createInParent(pChannelInfoAck->Info[0].Name, this, false);
	for (int i =0; i < pChannelInfoAck->Count; i++) {
		 
	}*/
}

void CharacterSelectLayer::CmdCreateCharacterAck(ICommand* cmd) {
	LoadCharacterInfoAck* pLoadCharacterInfoAck = cmd->CastCommand<LoadCharacterInfoAck*>();
}

void CharacterSelectLayer::CmdDeleteCharacterAck(ICommand* cmd) {
}
