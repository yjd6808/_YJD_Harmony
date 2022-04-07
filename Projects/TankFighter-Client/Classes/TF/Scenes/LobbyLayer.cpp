#include <TF/Scenes/LobbyLayer.h>
#include <TF/Object/Obstacle.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/UI/ColoredListView.h>
#include <TF/Network/GameClient.h>
#include <TF/Network/SendFn.h>

using namespace JCore;


void LobbyLayer::onEnterTransitionDidFinish() {
	/*
	 * �κ� �����ϸ� ���� ������ �������� ��û�ؾ��Ѵ�.
	 *
	 * 1. �ڽ��� ����
	 * 2. �渮��Ʈ
	 * 3. ģ�����
	 */
	if (SendFn::SendJoinLobbySyn() == false) {
		PopUp::createInParent("�κ� ���� ��û�� �����Ͽ����ϴ�.", this, false);
	}
}

bool LobbyLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	m_pRoomListView = ColoredListView::create(ColorUtil::To4B(ColorList::AoEnglish_v));
	m_pRoomListView->SetContantSize({ 600, Director::getInstance()->getWinSize().height });
	m_pRoomListView->setPosition(0, 0);
	m_pRoomListView->GetListView()->setInnerContainerPosition(Vec2::ZERO);
	m_pRoomListView->setAnchorPoint(Vec2::ZERO);
	m_pRoomListView->GetListView()->setPadding(10.0f, 10.0f, 10.0f, 10.0f);
	this->addChild(m_pRoomListView);

	m_pRoomTitleEditBox = EditBox::create(Size(200, 50), Scale9Sprite::create(RECT_IMG_FILENAME));
	m_pRoomTitleEditBox->setAnchorPoint(Vec2::ZERO);
	m_pRoomTitleEditBox->setPosition({ 600, 550 });
	m_pRoomTitleEditBox->setFontColor(Color4B::BLACK);
	m_pRoomTitleEditBox->setColor(ColorList::Africanviolet_v);
	m_pRoomTitleEditBox->setFontSize(15);
	m_pRoomTitleEditBox->setPlaceholderFontSize(15);
	m_pRoomTitleEditBox->setPlaceholderFontColor(Color3B::BLACK);
	m_pRoomTitleEditBox->setPlaceHolder("������ �� �̸��� �Է����ּ���.");
	m_pRoomTitleEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	this->addChild(m_pRoomTitleEditBox);

	m_pCreateRoomBtn = TextButton::create(200, 50, "�� �����", 16);
	m_pCreateRoomBtn->setBackgroundColor(ColorList::Babyblue_v);
	m_pCreateRoomBtn->setFontColor(ColorList::Bistrebrown_v);
	m_pCreateRoomBtn->setAnchorPoint(Vec2::ZERO);
	m_pCreateRoomBtn->setPosition({ 800, 550 });
	m_pCreateRoomBtn->setClickEvent(CC_CALLBACK_1(LobbyLayer::OnClickedCreateRoomButton, this));
	this->addChild(m_pCreateRoomBtn);

	m_pFriendNameEditBox = EditBox::create(Size(200, 50), Scale9Sprite::create(RECT_IMG_FILENAME));
	m_pFriendNameEditBox->setAnchorPoint(Vec2::ZERO);
	m_pFriendNameEditBox->setPosition({ 600, 500 });
	m_pFriendNameEditBox->setFontColor(Color4B::BLACK);
	m_pFriendNameEditBox->setColor(ColorList::BlackShadows_v);
	m_pFriendNameEditBox->setFontSize(15);
	m_pFriendNameEditBox->setPlaceholderFontSize(15);
	m_pFriendNameEditBox->setPlaceholderFontColor(Color3B::BLACK);
	m_pFriendNameEditBox->setPlaceHolder("ģ�� �̸��� �Է����ּ���.");
	m_pFriendNameEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	this->addChild(m_pFriendNameEditBox);

	m_AddFriendNameButton = TextButton::create(200, 50, "ģ�� �߰��ϱ�", 16);
	m_AddFriendNameButton->setBackgroundColor(ColorList::Babyblue_v);
	m_AddFriendNameButton->setFontColor(ColorList::Bistrebrown_v);
	m_AddFriendNameButton->setAnchorPoint(Vec2::ZERO);
	m_AddFriendNameButton->setPosition({ 800, 500 });
	m_AddFriendNameButton->setClickEvent(CC_CALLBACK_1(LobbyLayer::OnClickedAddFriendButton, this));
	this->addChild(m_AddFriendNameButton);

	m_pFriendListText = TextButton::create(400, 50, "ģ�� ���", 16);
	m_pFriendListText->setBackgroundColor(ColorList::Babyblue_v);
	m_pFriendListText->setFontColor(ColorList::Bistrebrown_v);
	m_pFriendListText->setAnchorPoint(Vec2::ZERO);
	m_pFriendListText->setPosition({ 600, 450 });
	m_pFriendListText->setClickEvent(CC_CALLBACK_1(LobbyLayer::OnClickedFriendListButton, this));
	this->addChild(m_pFriendListText);

	m_pFriendListView = ColoredListView::create(ColorUtil::To4B(ColorList::Blizzardblue_v));
	m_pFriendListView->SetContantSize({ 400, 250 });
	m_pFriendListView->setPosition(600, 200);
	m_pFriendListView->GetListView()->setInnerContainerPosition(Vec2::ZERO);
	m_pFriendListView->setAnchorPoint(Vec2::ZERO);
	m_pFriendListView->GetListView()->setPadding(10.0f, 10.0f, 10.0f, 10.0f);
	this->addChild(m_pFriendListView);

	m_pTerminateGameButton = TextButton::create(200, 50, "���� ����", 16);
	m_pTerminateGameButton->setBackgroundColor(ColorList::Cadmiumgreen_v);
	m_pTerminateGameButton->setFontColor(ColorList::Darkorange_v);
	m_pTerminateGameButton->setAnchorPoint(Vec2::ZERO);
	m_pTerminateGameButton->setPosition({ 600, 0 });
	m_pTerminateGameButton->setClickEvent(CC_CALLBACK_1(LobbyLayer::OnClickedTerminateGameButton, this));
	this->addChild(m_pTerminateGameButton);

	m_pSelectChannelButton = TextButton::create(200, 50, "ä�� ����", 16);
	m_pSelectChannelButton->setBackgroundColor(ColorList::Bluegray_v);
	m_pSelectChannelButton->setFontColor(ColorList::Darkorange_v);
	m_pSelectChannelButton->setAnchorPoint(Vec2::ZERO);
	m_pSelectChannelButton->setPosition({ 800, 0 });
	m_pSelectChannelButton->setClickEvent(CC_CALLBACK_1(LobbyLayer::OnClickedSelectChannelButton, this));
	this->addChild(m_pSelectChannelButton);


	m_pNyInfoButton = TextButton::create(200, 150, "�� ����", 16);
	m_pNyInfoButton->setBackgroundColor(ColorList::Blackcoral_v);
	m_pNyInfoButton->setFontColor(ColorList::BlackShadows_v);
	m_pNyInfoButton->setAnchorPoint(Vec2::ZERO);
	m_pNyInfoButton->setPosition({ 600, 50 });
	m_pNyInfoButton->setClickEvent(CC_CALLBACK_1(LobbyLayer::OnClickedMyInfoButton, this));
	this->addChild(m_pNyInfoButton);

	return true;
}



// �� ���� / CREATE_ROOM_SYN 120
void LobbyLayer::OnClickedCreateRoomButton(TextButton* btn) {
	const JCore::String roomName = m_pRoomTitleEditBox->getText();

	if (roomName.Length() == 0) {
		PopUp::createInParent("������ �� �̸��� �Է����ּ���.", this, false);
		return;
	}

	if (roomName.Length() + 1 >= NAME_LEN) {
		PopUp::createInParent("�� �̸��� 40����Ʈ ���Ϸ� �Է����ּ���", this, false);
		return;
	}

	SendFn::SendCreateRoomSyn(roomName);
}

// ���� ����
void LobbyLayer::OnClickedTerminateGameButton(TextButton* btn) {

	if (_Client->Disconnect()) {
		Director::getInstance()->end();
	}
}

// ä�� ����
void LobbyLayer::OnClickedSelectChannelButton(TextButton* btn) {
	_Client->SetChannelUID(INVALID_UID);
	_Client->SetCharacterUID(INVALID_UID);
	_Client->SetRoomUID(INVALID_UID);

	this->unscheduleUpdate();
	this->_eventDispatcher->removeAllEventListeners();
	Director::getInstance()->replaceScene(ChannelScene::createScene());
}

// �� ���� / JOIN_ROOM_SYN 121
void LobbyLayer::OnClickedJoinRoomButton(TextButton* btn) {

	// ��ư�� ������ �±׿� �ش� �����ϰ��� �ϴ� ���� CharacterUID ������ �����س���
	SendFn::SendJoinRoomSyn(btn->getTag());
}


// �� ��� Ŭ����
void LobbyLayer::OnClickedFriendListButton(TextButton* btn) {
	PopUp::createInParent("������ : ������", this, false);
}

// ģ�� �߰� / ADD_FRIEND_SYN 122
void LobbyLayer::OnClickedAddFriendButton(TextButton* btn) {
	const JCore::String friendName = m_pFriendNameEditBox->getText();

	if (friendName.Length() == 0) {
		PopUp::createInParent("ģ�� �߰��� �г����� �Է����ּ���.", this, false);
		return;
	}

	if (friendName.Length() + 1 >= NAME_LEN) {
		PopUp::createInParent("ģ�� �߰��� �г��� �̸��� 40����Ʈ ���Ϸ� �Է����ּ���", this, false);
		return;
	}

	SendFn::SendAddFriendSyn(friendName);
}

// ģ�� ���� / DELETE_FRIEND_SYN 123
void LobbyLayer::OnClickedDeleteFriendButton(TextButton* btn) {

	// ��ư�� ������ �±׿� �ش� ĳ���� CharacterUID ������ �����س���
	SendFn::SendDeleteFriendSyn(btn->getTag());

	
}

void LobbyLayer::OnClickedMyInfoButton(TextButton* btn) {
	PopUp::createInParent("����", this, false);
}


/* =================================================================================
 *
 *                             ��� ��Ŷ ó��
 *
 * ================================================================================*/

 // UPDATE_CHARACTER_INFO_ACK 117
void LobbyLayer::CmdUpdateCharacterInfoAck(ICommand* cmd) {
	UpdateCharacterInfoAck* pUpdateCharacterInfoAck = cmd->CastCommand<UpdateCharacterInfoAck*>();

	// ĳ���� ���� ������Ʈ
	if (pUpdateCharacterInfoAck->Result) {
		_Client->UpdateCharacterInfo(pUpdateCharacterInfoAck->Info);

		m_pNyInfoButton->setText(StringUtils::format("%s\n%dų %d����\n%d�¸� %d�й�\n%d ���",
			pUpdateCharacterInfoAck->Info.Name,
			pUpdateCharacterInfoAck->Info.Kill,
			pUpdateCharacterInfoAck->Info.Death,
			pUpdateCharacterInfoAck->Info.Win,
			pUpdateCharacterInfoAck->Info.Lose,
			pUpdateCharacterInfoAck->Info.Money
		));
		return;
	}

	PopUp::createInParent(pUpdateCharacterInfoAck->Reason, this, false, [this]() {
		_Client->SetChannelUID(INVALID_UID);
		_Client->SetCharacterUID(INVALID_UID);
		_Client->SetRoomUID(INVALID_UID);

		this->unscheduleUpdate();
		this->_eventDispatcher->removeAllEventListeners();
		Director::getInstance()->replaceScene(ChannelScene::createScene());
	});
}


// UPDATE_ROOMLIST_ACK 118
void LobbyLayer::CmdUpdateRoomListAck(ICommand* cmd) {
	const UpdateRoomListAck* pUpdateRoomListAck = cmd->CastCommand<UpdateRoomListAck*>();
	m_pRoomListView->GetListView()->removeAllItems();

	for (int i = 0; i < pUpdateRoomListAck->Count; i++) {
		const RoomInfo* pInfo = &pUpdateRoomListAck->Info[i];
		const auto pRoomButton = TextButton::create(580, 70, StringUtils::format("%s (%d/%d)", pInfo->Name, pInfo->PlayerCount, pInfo->MaxPlayerCount), 16);
		pRoomButton->setAnchorPoint(Vec2::ZERO);
		pRoomButton->setBackgroundColor(ColorList::Aero_v);
		pRoomButton->setTag(pInfo->RoomUID);	// ���ȣ CharacterUID ����
		pRoomButton->setClickEvent(CC_CALLBACK_1(LobbyLayer::OnClickedJoinRoomButton, this));
		m_pRoomListView->GetListView()->pushBackCustomItem(pRoomButton);
	}
}


// UPDATE_FRIENDLIST_ACK 119
void LobbyLayer::CmdUpdateFriendListAck(ICommand* cmd) {
	const UpdateFriendListAck* pUpdateFriendListAck = cmd->CastCommand<UpdateFriendListAck*>();

	m_pFriendListView->GetListView()->removeAllItems();

	for (int i = 0; i < pUpdateFriendListAck->Count; i++) {
		const CharacterInfo* pInfo = &pUpdateFriendListAck->Info[i];
		const auto pFriendButton = TextButton::create(580, 70, pInfo->Name, 16);
		pFriendButton->setAnchorPoint(Vec2::ZERO);
		pFriendButton->setBackgroundColor(ColorList::Aero_v);
		pFriendButton->setTag(pInfo->CharacterUID);	// ���ȣ CharacterUID ����
		pFriendButton->setClickEvent(CC_CALLBACK_1(LobbyLayer::OnClickedDeleteFriendButton, this));
		m_pFriendListView->GetListView()->pushBackCustomItem(pFriendButton);
	}
	
}

void LobbyLayer::CmdCreateRoomAck(ICommand* cmd) {
	const CreateRoomAck* pCreateRoomAck = cmd->CastCommand<CreateRoomAck*>();

	if (pCreateRoomAck->Result) {
		_Client->SetRoomUID(pCreateRoomAck->RoomUID);

		this->unscheduleUpdate();
		this->_eventDispatcher->removeAllEventListeners();
		Director::getInstance()->replaceScene(RoomScene::createScene());
		return;
	}

	PopUp::createInParent(pCreateRoomAck->Reason, this, false);
	
}

void LobbyLayer::CmdJoinRoomAck(ICommand* cmd) {
	const JoinRoomAck* pJoinRoomAck = cmd->CastCommand<JoinRoomAck*>();

	if (pJoinRoomAck->Result) {
		_Client->SetRoomUID(pJoinRoomAck->RoomUID);

		this->unscheduleUpdate();
		this->_eventDispatcher->removeAllEventListeners();
		Director::getInstance()->replaceScene(RoomScene::createScene());
		return;
	}

	PopUp::createInParent(pJoinRoomAck->Reason, this, false);
}

// ģ�� �߰� ��û
void LobbyLayer::CmdAddFriendAck(ICommand* cmd) {
	const AddFriendAck* pUpdateFriendListAck = cmd->CastCommand<AddFriendAck*>();
	PopUp::createInParent(pUpdateFriendListAck->Reason, this, false);
}

// ���濡�� ��û ����
void LobbyLayer::CmdAddFriendRequestSyn(ICommand* cmd) {
	const AddFriendRequestSyn* pAddFriendRequestSyn = cmd->CastCommand<AddFriendRequestSyn*>();
	// �̰� ��Ŷ�� �ð� ������ ������Ƿ� �����ؼ� �����ؾ���
	int iRequestCharacterUID = pAddFriendRequestSyn->RequestCharacterUID;

	PopUp::createInParent(StringUtils::format("%s ���� ģ�� ��û�� ���½��ϴ�. �����Ͻʴϱ�?", pAddFriendRequestSyn->Info.Name).c_str(), this, false,
		[iRequestCharacterUID]() { SendFn::SendAddFriendRequestAck(iRequestCharacterUID, true);  },	 // ����
		[iRequestCharacterUID]() { SendFn::SendAddFriendRequestAck(iRequestCharacterUID, false); }); // ���� 
}
