/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:32:29 PM
 * =====================
 *
 */

#include "Pch.h"
#include "LobbyScene.h"

#include <TF/Common/Structure.h>
#include <TF/Client/Net/Send/S_GAME.h>
#include <TF/Client/Game/Scene/ChannelScene.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_CCUI;

LobbyScene::LobbyScene()
	: m_pChatBox(nullptr)
	, m_pRoomListText(nullptr)
	, m_pPlayerListText(nullptr)
	, m_pRoomListView(nullptr)
	, m_pPlayerListView(nullptr)
	, m_pRoomTitleEditBox(nullptr)
	, m_pCreateRoomBtn(nullptr)
	, m_pFriendNameEditBox(nullptr)
	, m_AddFriendNameButton(nullptr)
	, m_pFriendListText(nullptr)
	, m_pFriendListView(nullptr)
	, m_pTerminateGameButton(nullptr)
	, m_pSelectChannelButton(nullptr)
	, m_pSelectCharacterButton(nullptr)
	, m_pNyInfoButton(nullptr)
{}

LobbyScene::~LobbyScene()
{}

void LobbyScene::onEnterTransitionDidFinish() {
	BaseScene::onEnterTransitionDidFinish();

	if (S_GAME::SEND_CS_LoadLobbyInfo() == false) {
		PopUp::createInParent("캐릭터 정보 요청이 실패하였습니다.", this, false);
	}
}

bool LobbyScene::init() {

	if (!BaseScene::init()) {
		return false;
	}

	m_pRoomListText = TextButton::create(400, 50, "생성된 방 목록", 16);
	m_pRoomListText->setBackgroundColor(ColorList::Darkorange_v);
	m_pRoomListText->setFontColor(ColorList::Black_v);
	m_pRoomListText->setAnchorPoint(Vec2::ZERO);
	m_pRoomListText->setPosition({ 0, 550 });
	m_pUILayer->addChild(m_pRoomListText);

	m_pPlayerListText = TextButton::create(200, 50, "접속중인 유저 목록", 16);
	m_pPlayerListText->setBackgroundColor(ColorList::Darkorange_v);
	m_pPlayerListText->setFontColor(ColorList::Black_v);
	m_pPlayerListText->setAnchorPoint(Vec2::ZERO);
	m_pPlayerListText->setPosition({ 400, 550 });
	m_pUILayer->addChild(m_pPlayerListText);


	m_pRoomListView = ColoredListView::create(ColorUtil::To4B(ColorList::AoEnglish_v));
	m_pRoomListView->SetContantSize({ 400, Director::getInstance()->getWinSize().height - 250.0f });
	m_pRoomListView->setPosition(0, 200.0);
	m_pRoomListView->GetListView()->setInnerContainerPosition(Vec2::ZERO);
	m_pRoomListView->setAnchorPoint(Vec2::ZERO);
	m_pRoomListView->GetListView()->setPadding(10.0f, 10.0f, 10.0f, 10.0f);
	m_pUILayer->addChild(m_pRoomListView);

	m_pPlayerListView = ColoredListView::create(ColorUtil::To4B(ColorList::Almond_v));
	m_pPlayerListView->SetContantSize({ 200, Director::getInstance()->getWinSize().height - 250.0f });
	m_pPlayerListView->setPosition(400, 200.0);
	m_pPlayerListView->GetListView()->setInnerContainerPosition(Vec2::ZERO);
	m_pPlayerListView->setAnchorPoint(Vec2::ZERO);
	m_pPlayerListView->GetListView()->setPadding(10.0f, 10.0f, 10.0f, 10.0f);
	m_pUILayer->addChild(m_pPlayerListView);

	m_pRoomTitleEditBox = EditBox::create(Size(200, 50), Scale9Sprite::create(Const::Resource::WhiteRectFileName));
	m_pRoomTitleEditBox->addClickEventListener([](Ref* ref) { ((EditBox*)ref)->openKeyboard(); });
	m_pRoomTitleEditBox->setAnchorPoint(Vec2::ZERO);
	m_pRoomTitleEditBox->setPosition({ 600, 550 });
	m_pRoomTitleEditBox->setFontColor(Color4B::BLACK);
	m_pRoomTitleEditBox->setColor(ColorList::Africanviolet_v);
	m_pRoomTitleEditBox->setFontSize(15);
	m_pRoomTitleEditBox->setPlaceholderFontSize(15);
	m_pRoomTitleEditBox->setPlaceholderFontColor(Color3B::BLACK);
	m_pRoomTitleEditBox->setPlaceHolder("생성할 방 이름을 입력해주세요.");
	m_pRoomTitleEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	m_pUILayer->addChild(m_pRoomTitleEditBox);

	m_pCreateRoomBtn = TextButton::create(200, 50, "방 만들기", 16);
	m_pCreateRoomBtn->setBackgroundColor(ColorList::Babyblue_v);
	m_pCreateRoomBtn->setFontColor(ColorList::Bistrebrown_v);
	m_pCreateRoomBtn->setAnchorPoint(Vec2::ZERO);
	m_pCreateRoomBtn->setPosition({ 800, 550 });
	m_pCreateRoomBtn->setClickEvent(CC_CALLBACK_1(LobbyScene::onClickedCreateRoomButton, this));
	m_pUILayer->addChild(m_pCreateRoomBtn);

	m_pFriendNameEditBox = EditBox::create(Size(200, 50), Scale9Sprite::create(Const::Resource::WhiteRectFileName));
	m_pFriendNameEditBox->addClickEventListener([](Ref* ref) { ((EditBox*)ref)->openKeyboard(); });
	m_pFriendNameEditBox->setAnchorPoint(Vec2::ZERO);
	m_pFriendNameEditBox->setPosition({ 600, 500 });
	m_pFriendNameEditBox->setFontColor(Color4B::BLACK);
	m_pFriendNameEditBox->setColor(ColorList::BlackShadows_v);
	m_pFriendNameEditBox->setFontSize(15);
	m_pFriendNameEditBox->setPlaceholderFontSize(15);
	m_pFriendNameEditBox->setPlaceholderFontColor(Color3B::BLACK);
	m_pFriendNameEditBox->setPlaceHolder("친구 이름을 입력해주세요.");
	m_pFriendNameEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	m_pUILayer->addChild(m_pFriendNameEditBox);

	m_AddFriendNameButton = TextButton::create(200, 50, "친구 추가하기", 16);
	m_AddFriendNameButton->setBackgroundColor(ColorList::Babyblue_v);
	m_AddFriendNameButton->setFontColor(ColorList::Bistrebrown_v);
	m_AddFriendNameButton->setAnchorPoint(Vec2::ZERO);
	m_AddFriendNameButton->setPosition({ 800, 500 });
	m_AddFriendNameButton->setClickEvent(CC_CALLBACK_1(LobbyScene::onClickedAddFriendButton, this));
	m_pUILayer->addChild(m_AddFriendNameButton);

	m_pFriendListText = TextButton::create(400, 50, "친구 목록", 16);
	m_pFriendListText->setBackgroundColor(ColorList::Bittersweet_v);
	m_pFriendListText->setFontColor(ColorList::Black_v);
	m_pFriendListText->setAnchorPoint(Vec2::ZERO);
	m_pFriendListText->setPosition({ 600, 450 });
	m_pFriendListText->setClickEvent(CC_CALLBACK_1(LobbyScene::onClickedFriendListButton, this));
	m_pUILayer->addChild(m_pFriendListText);

	m_pFriendListView = ColoredListView::create(ColorUtil::To4B(ColorList::Blizzardblue_v));
	m_pFriendListView->SetContantSize({ 400, 250 });
	m_pFriendListView->setPosition(600, 200);
	m_pFriendListView->GetListView()->setInnerContainerPosition(Vec2::ZERO);
	m_pFriendListView->setAnchorPoint(Vec2::ZERO);
	m_pFriendListView->GetListView()->setPadding(10.0f, 10.0f, 10.0f, 10.0f);
	m_pUILayer->addChild(m_pFriendListView);

	m_pTerminateGameButton = TextButton::create(200, 50, "게임 종료", 16);
	m_pTerminateGameButton->setBackgroundColor(ColorList::Beaublue_v);
	m_pTerminateGameButton->setFontColor(ColorList::Black_v);
	m_pTerminateGameButton->setAnchorPoint(Vec2::ZERO);
	m_pTerminateGameButton->setPosition({ 600, 0 });
	m_pTerminateGameButton->setClickEvent(CC_CALLBACK_1(LobbyScene::onClickedTerminateGameButton, this));
	m_pUILayer->addChild(m_pTerminateGameButton);

	m_pSelectChannelButton = TextButton::create(200, 50, "채널 나가기", 16);
	m_pSelectChannelButton->setBackgroundColor(ColorList::Beaublue_v);
	m_pSelectChannelButton->setFontColor(ColorList::Black_v);
	m_pSelectChannelButton->setAnchorPoint(Vec2::ZERO);
	m_pSelectChannelButton->setPosition({ 800, 0 });
	m_pSelectChannelButton->setClickEvent(CC_CALLBACK_1(LobbyScene::onClickedSelectChannelButton, this));
	m_pUILayer->addChild(m_pSelectChannelButton);

	m_pSelectCharacterButton = TextButton::create(200, 50, "로비 나가기", 16);
	m_pSelectCharacterButton->setBackgroundColor(ColorList::Beaublue_v);
	m_pSelectCharacterButton->setFontColor(ColorList::Black_v);
	m_pSelectCharacterButton->setAnchorPoint(Vec2::ZERO);
	m_pSelectCharacterButton->setPosition({ 800, 50 });
	m_pSelectCharacterButton->setClickEvent(CC_CALLBACK_1(LobbyScene::onClickedSelectCharacterButton, this));
	m_pUILayer->addChild(m_pSelectCharacterButton);

	m_pNyInfoButton = TextButton::create(200, 100, "내 정보", 16);
	m_pNyInfoButton->setBackgroundColor(ColorList::Blackcoral_v);
	m_pNyInfoButton->setFontColor(ColorList::BlackShadows_v);
	m_pNyInfoButton->setAnchorPoint(Vec2::ZERO);
	m_pNyInfoButton->setPosition({ 600, 50 });
	m_pNyInfoButton->setClickEvent(CC_CALLBACK_1(LobbyScene::onClickedMyInfoButton, this));
	m_pUILayer->addChild(m_pNyInfoButton);

	m_pChatBox = ChatBox::create(ColorList::BlackShadows_v, { 600, 200 });
	m_pChatBox->setAnchorPoint(Vec2::ZERO);
	m_pChatBox->SetSendButtonClickEvent(CC_CALLBACK_1(LobbyScene::onClickedChatSendButton, this));
	m_pChatBox->SetContentSize({ 600, 200 });
	m_pUILayer->addChild(m_pChatBox);

	return true;
}


void LobbyScene::onClickedChatSendButton(ChatBox* chatBox) {
	JCore::String message = chatBox->GetEditBox()->getText();
	if (message.Length() == 0) {
		return;
	}
	S_GAME::SEND_CS_ChatMessage(message.Source());
}

// 방 생성
void LobbyScene::onClickedCreateRoomButton(TextButton* btn) {
	const JCore::String roomName = m_pRoomTitleEditBox->getText();

	if (roomName.Length() == 0) {
		PopUp::createInParent("생성할 방 이름을 입력해주세요.", this, false);
		return;
	}

	if (roomName.Length() + 1 >= Const::Length::Name) {
		PopUp::createInParent("방 이름은 40바이트 이하로 입력해주세요", this, false);
		return;
	}

	S_GAME::SEND_CS_CreateRoom(roomName);
}

// 게임 종료
void LobbyScene::onClickedTerminateGameButton(TextButton* btn) {

	if (Core::GameClient->Disconnect()) {
		Director::getInstance()->end();
	}
}

// 채널 선택
void LobbyScene::onClickedSelectChannelButton(TextButton* btn) {
	S_GAME::SEND_CS_LeaveChannel(Core::GameClient->GetChannelPrimaryKey());
}

void LobbyScene::onClickedSelectCharacterButton(TextButton* btn) {
	S_GAME::SEND_CS_LeaveLobby();
}

// 방 참가
void LobbyScene::onClickedJoinRoomButton(TextButton* btn) {

	// 버튼에 지정된 태그에 해당 참가하고자 하는 방의 엑세스키 정보를 저장해놓음
	S_GAME::SEND_CS_JoinRoom(btn->getTag());
}


// 방 목록 클릭시
void LobbyScene::onClickedFriendListButton(TextButton* btn) {
	PopUp::createInParent("개발자 : 윤정도", this, false);
}

void LobbyScene::onClickedAddFriendButton(TextButton* btn) {
	const JCore::String friendName = m_pFriendNameEditBox->getText();

	if (friendName.Length() == 0) {
		PopUp::createInParent("친구 추가할 닉네임을 입력해주세요.", this, false);
		return;
	}

	if (friendName.Length() + 1 >= Const::Length::Name) {
		PopUp::createInParent("친구 추가할 닉네임 이름은 40바이트 이하로 입력해주세요", this, false);
		return;
	}

	S_GAME::SEND_CS_AddFriend(friendName.Source());
}

void LobbyScene::onClickedDeleteFriendButton(TextButton* btn) {
	int iTargetPrimaryKey = btn->getTag();
	PopUp::createInRunningScene("정말로 친구 삭제하시겠습니까?", false,
		[iTargetPrimaryKey] { S_GAME::SEND_CS_DeleteFriend(iTargetPrimaryKey); },
		[iTargetPrimaryKey] {}
	);
}

void LobbyScene::onClickedMyInfoButton(TextButton* btn) {
	PopUp::createInParent("ㄷㄷ", this, false);
}


void LobbyScene::refreshPlayerList(CharacterInfo* characterList, int count) {
	if (m_pPlayerListView == nullptr)
		return;

	m_pPlayerListView->GetListView()->removeAllItems();

	for (int i = 0; i < count; ++i) {
		const CharacterInfo& info = characterList[i];
		const auto pPlayerBtn = TextButton::create(150, 40, info.Name.Source, 16);
		pPlayerBtn->setAnchorPoint(Vec2::ZERO);
		pPlayerBtn->setBackgroundColor(info.LoggedIn ? ColorList::Acidgreen_v : ColorList::Aero_v);
		pPlayerBtn->setTag(info.PrimaryKey);	// 태그를 캐릭터 PrimaryKey로 지정
		m_pPlayerListView->GetListView()->pushBackCustomItem(pPlayerBtn);
	}
}


void LobbyScene::refreshCharacterInfo(const CharacterInfo& info) {
	m_pNyInfoButton->setText(StringUtils::format("%s\n%d킬 %d데스\n%d승리 %d패배\n%d 골드",
		info.Name.Source,
		info.Kill,
		info.Death,
		info.Win,
		info.Lose,
		info.Money
	));
}

void LobbyScene::refreshFriendList(FriendCharacterInfo* characterList, int count) {
	if (m_pFriendListView == nullptr)
		return;

	m_pFriendListView->GetListView()->removeAllItems();

	for (int i = 0; i < count; i++) {
		const FriendCharacterInfo& info = characterList[i];
		const auto pFriendButton = TextButton::create(350, 40, info.LoggedIn ? StringUtils::format("%s (접속중)", info.Name.Source) : info.Name.Source , 16);
		pFriendButton->setAnchorPoint(Vec2::ZERO);
		pFriendButton->setBackgroundColor(info.LoggedIn ? ColorList::Acidgreen_v : ColorList::Aero_v);
		pFriendButton->setTag(info.PrimaryKey);	// 태그를 캐릭터 PrimaryKey로 지정
		pFriendButton->setClickEvent(CC_CALLBACK_1(LobbyScene::onClickedDeleteFriendButton, this));
		m_pFriendListView->GetListView()->pushBackCustomItem(pFriendButton);
	}
}

void LobbyScene::refreshRoomList(RoomInfo* roomList, int count) {
	if (m_pRoomListView == nullptr)
		return;

	m_pRoomListView->GetListView()->removeAllItems();

	for (int i = 0; i < count; i++) {
		const RoomInfo& info = roomList[i];
		const auto pRoomButton = TextButton::create(350, 40, StringUtils::format("%s (%d/%d)", info.Name.Source, info.PlayerCount, info.MaxPlayerCount), 16);
		pRoomButton->setAnchorPoint(Vec2::ZERO);
		pRoomButton->setBackgroundColor(info.PlayerCount >= info.MaxPlayerCount ? ColorList::Babypink_v : ColorList::Aero_v);
		pRoomButton->setTag(info.AccessId);	// 태그를 방 AccessId로 지정
		pRoomButton->setClickEvent(CC_CALLBACK_1(LobbyScene::onClickedJoinRoomButton, this));
		m_pRoomListView->GetListView()->pushBackCustomItem(pRoomButton);
	}
}

void LobbyScene::addChatMssage(const char* str) {
	m_pChatBox->AddChatMessage(str);
}
