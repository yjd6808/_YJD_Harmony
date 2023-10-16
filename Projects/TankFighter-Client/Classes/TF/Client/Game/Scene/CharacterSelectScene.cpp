/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:32:29 PM
 * =====================
 *
 */

#include "Pch.h"
#include "CharacterSelectScene.h"

#include <TF/Common/Structure.h>
#include <TF/Client/Net/Send/S_GAME.h>
#include <TF/Client/Game/Scene/ChannelScene.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_CCUI;

CharacterSelectScene::CharacterSelectScene()
{}

CharacterSelectScene::~CharacterSelectScene()
{}

void CharacterSelectScene::onEnterTransitionDidFinish() {
	BaseScene::onEnterTransitionDidFinish();

	if (S_GAME::SEND_CS_LoadCharacterInfo(Core::GameClient->GetAccountPrimaryKey(), Core::GameClient->GetChannelPrimaryKey()) == false) {
		PopUp::createInParent("캐릭터 정보 요청이 실패하였습니다.", this, false);
	}
}

bool CharacterSelectScene::init() {

	if (!BaseScene::init()) {
		return false;
	}


	m_pCharacterNickNameEditBox = EditBox::create(Size(200, 45), Scale9Sprite::create(Const::Resource::WhiteRectFileName));
	m_pCharacterNickNameEditBox->addClickEventListener([](Ref* ref) { ((EditBox*)ref)->openKeyboard(); });
	m_pCharacterNickNameEditBox->setPosition({ 600, 350 });
	m_pCharacterNickNameEditBox->setFontColor(Color4B::WHITE);
	m_pCharacterNickNameEditBox->setColor(ColorList::Africanviolet_v);
	m_pCharacterNickNameEditBox->setFontSize(15);
	m_pCharacterNickNameEditBox->setPlaceHolder("삭제 또는 생성할 캐릭터명 입력");
	m_pCharacterNickNameEditBox->setPlaceholderFontSize(15);
	m_pCharacterNickNameEditBox->setAnchorPoint(Vec2::ZERO);
	m_pCharacterNickNameEditBox->setPlaceholderFontColor(Color4B::BLACK);
	m_pCharacterNickNameEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	this->addChild(m_pCharacterNickNameEditBox);

	m_pCreateCharacterButton = TextButton::create(200, 45, "캐릭터 생성", 16);
	m_pCreateCharacterButton->setPosition({ 600, 300 });
	m_pCreateCharacterButton->setAnchorPoint(Vec2::ZERO);
	m_pCreateCharacterButton->setColor(ColorList::Babypink_v);
	m_pCreateCharacterButton->setFontColor(ColorList::Black_v);
	m_pCreateCharacterButton->setClickEvent(CC_CALLBACK_1(CharacterSelectScene::onClickedCreateCharacterButton, this));
	this->addChild(m_pCreateCharacterButton);

	m_pDeleteCharacterButton = TextButton::create(200, 45, "캐릭터 삭제", 16);
	m_pDeleteCharacterButton->setPosition({ 600, 250 });
	m_pDeleteCharacterButton->setAnchorPoint(Vec2::ZERO);
	m_pDeleteCharacterButton->setColor(ColorList::Babypink_v);
	m_pDeleteCharacterButton->setFontColor(ColorList::Black_v);
	m_pDeleteCharacterButton->setClickEvent(CC_CALLBACK_1(CharacterSelectScene::onClickedDeleteCharacterButton, this));
	this->addChild(m_pDeleteCharacterButton);

	m_pChannelSelectButton = TextButton::create(200, 45, "채널 선택", 16);
	m_pChannelSelectButton->setPosition({ 600, 200 });
	m_pChannelSelectButton->setAnchorPoint(Vec2::ZERO);
	m_pChannelSelectButton->setColor(ColorList::Babypink_v);
	m_pChannelSelectButton->setFontColor(ColorList::Black_v);
	m_pChannelSelectButton->setClickEvent(CC_CALLBACK_1(CharacterSelectScene::onClickedChannelSelectButton, this));
	this->addChild(m_pChannelSelectButton);

	this->scheduleUpdate();
	return true;
}


bool CharacterSelectScene::isValidNickName(std::string& nick) {
	for (int i = 0; i < nick.size(); i++) {
		if (nick[i] == ' ') {
			return false;
		}
	}

	return true;
}

void CharacterSelectScene::onClickedCharacterButton(TextButton* btn) {
	// 버튼의 태그에 캐릭터 CharacterUID 정보가 들어있으므로
	S_GAME::SEND_CS_SelectCharacter(btn->getTag());
}


void CharacterSelectScene::onClickedCreateCharacterButton(TextButton* sender) {

	std::string nickName = m_pCharacterNickNameEditBox->getText();

	if (nickName.length() == 0) {
		PopUp::createInParent("생성할 캐릭터 명을 입력해주세요.", this, false);
		return;
	}

	if (!isValidNickName(nickName)) {
		PopUp::createInParent("공백이 포함되어 있습니다", this, false);
		return;
	}

	if (nickName.length() + 1 >= Const::Length::Name) {
		PopUp::createInParent("닉네임은 40바이트 이하로 입력해주세요", this, false);
		return;
	}

	S_GAME::SEND_CS_CreateCharacter(nickName.c_str());
}


void CharacterSelectScene::onClickedDeleteCharacterButton(TextButton* sender) {

	std::string nickName = m_pCharacterNickNameEditBox->getText();

	if (nickName.length() == 0) {
		PopUp::createInParent("삭제할 캐릭터 명을 입력해주세요.", this, false);
		return;
	}

	if (!isValidNickName(nickName)) {
		PopUp::createInParent("공백이 포함되어 있습니다", this, false);
		return;
	}

	if (nickName.length() + 1 >= Const::Length::Name) {
		PopUp::createInParent("닉네임은 40바이트 이하로 입력해주세요", this, false);
		return;
	}

	S_GAME::SEND_CS_DeleteCharacter(nickName.c_str());
}

void CharacterSelectScene::onClickedChannelSelectButton(TextButton* sender) {
	S_GAME::SEND_CS_LeaveChannel(Core::GameClient->GetChannelPrimaryKey());
}

void CharacterSelectScene::refreshCharacterList(CharacterInfo* characterList, int count) {

	// 캐릭터 정보가 갱신될 수 있으므로 먼저 다 비워준다.
	for (int i = 0; i < m_vCharacterSelectButtonList.Size(); i++) {
		m_pUILayer->removeChild(m_vCharacterSelectButtonList[i]);
	}
	m_vCharacterSelectButtonList.Clear();

	if (count <= 0) {
		PopUp::createInParent("캐릭터가 없습니다. 생성해주세요.", this, false);
		return;
	}

	float iXpos = 100;
	float iYpos = 400;

	for (int i = 0, j = 0; i < count; i++, j++) {
		CharacterInfo* info = &characterList[i];

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
		TextButton* pCharacterBtn = TextButton::create(190, 45, StringUtils::format("%s\n%d Gold", info->Name.Source, info->Money), 16);
		pCharacterBtn->setPosition({ iXpos, iYpos });
		pCharacterBtn->setBackgroundColor(ColorList::Ashgray_v);
		pCharacterBtn->setFontColor(ColorList::Black_v);
		pCharacterBtn->setAnchorPoint(Vec2::ZERO);
		pCharacterBtn->setClickEvent(CC_CALLBACK_1(CharacterSelectScene::onClickedCharacterButton, this));
		m_pUILayer->addChild(pCharacterBtn, 0, info->PrimaryKey);
		m_vCharacterSelectButtonList.PushBack(pCharacterBtn);
	}
}

