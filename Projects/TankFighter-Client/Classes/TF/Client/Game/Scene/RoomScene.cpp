/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:32:29 PM
 * =====================
 *
 */

#include "Pch.h"
#include "RoomScene.h"

#include <TF/Common/Structure.h>
#include <TF/Common/Const.h>
#include <TF/Client/Net/Send/S_GAME.h>
#include <TF/Client/Game/Scene/ChannelScene.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_CCUI;

RoomScene::RoomScene()
	: m_pGameStartBtn(nullptr)
	, m_pGameReadyBtn(nullptr)
	, m_pGameReadyCancelBtn(nullptr)
	, m_pLeaveRoomBtn(nullptr)
	, m_pRoomTitle(nullptr)
	, m_pSlot()
	, m_pMarkSlot()
	, m_iHostCharacterPrimaryKey(Const::InvalidValue)
	, m_RoomMember()
{}

RoomScene::~RoomScene()
{}

void RoomScene::onEnterTransitionDidFinish() {
	BaseScene::onEnterTransitionDidFinish();

	if (S_GAME::SEND_CS_LoadRoomInfo(Core::GameClient->GetRoomAccessId()) == false) {
		PopUp::createInParent("방 정보 요청이 실패하였습니다.", this, false);
	}
}


bool RoomScene::init() {

	if (!BaseScene::init()) {
		return false;
	}

	// 700 350 - 게임 시작
	// 700 300 - 시작 준비
	// 700 250 - 준비 해제
	// 700 200 - 방 나가기

	m_pGameStartBtn = TextButton::create(200, 45, "게임 시작", 15);
	m_pGameStartBtn->setAnchorPoint(Vec2::ZERO);
	m_pGameStartBtn->setPosition({ 700, 350 });
	m_pGameStartBtn->setBackgroundColor(ColorList::Babypink_v);
	m_pGameStartBtn->setFontColor(ColorList::Blackbean_v);
	m_pGameStartBtn->setClickEvent(CC_CALLBACK_1(RoomScene::onClickedGameStartButton, this));
	m_pUILayer->addChild(m_pGameStartBtn);

	m_pGameReadyBtn = TextButton::create(200, 45, "시작 준비", 15);
	m_pGameReadyBtn->setAnchorPoint(Vec2::ZERO);
	m_pGameReadyBtn->setPosition({ 700, 300 });
	m_pGameReadyBtn->setBackgroundColor(ColorList::Ashgray_v);
	m_pGameReadyBtn->setFontColor(ColorList::Bistre_v);
	m_pGameReadyBtn->setClickEvent(CC_CALLBACK_1(RoomScene::onClickedGameReadyButton, this));
	m_pUILayer->addChild(m_pGameReadyBtn);

	m_pGameReadyCancelBtn = TextButton::create(200, 45, "준비 해제", 15);
	m_pGameReadyCancelBtn->setAnchorPoint(Vec2::ZERO);
	m_pGameReadyCancelBtn->setPosition({ 700, 250 });
	m_pGameReadyCancelBtn->setBackgroundColor(ColorList::Ashgray_v);
	m_pGameReadyCancelBtn->setFontColor(ColorList::Bistre_v);
	m_pGameReadyCancelBtn->setClickEvent(CC_CALLBACK_1(RoomScene::onClickedGameReadyCancelButton, this));
	m_pUILayer->addChild(m_pGameReadyCancelBtn);

	m_pLeaveRoomBtn = TextButton::create(200, 45, "방 나가기", 15);
	m_pLeaveRoomBtn->setAnchorPoint(Vec2::ZERO);
	m_pLeaveRoomBtn->setPosition({ 700, 200 });
	m_pLeaveRoomBtn->setBackgroundColor(ColorList::Ashgray_v);
	m_pLeaveRoomBtn->setFontColor(ColorList::Bistre_v);
	m_pLeaveRoomBtn->setClickEvent(CC_CALLBACK_1(RoomScene::onClickedGameLeaveRoomButton, this));
	m_pUILayer->addChild(m_pLeaveRoomBtn);

	for (int i = 0; i < Const::Room::MaxPlayerCount; i++) {
		m_pSlot[i] = TextButton::create(180, 135, "닉네임", 15);
		m_pSlot[i]->setAnchorPoint(Vec2::ZERO);
		m_pSlot[i]->setBackgroundColor(ColorList::Ashgray_v);
		m_pSlot[i]->setFontColor(ColorList::Bistre_v);
		m_pSlot[i]->setVisible(false);
		m_pUILayer->addChild(m_pSlot[i]);

		m_pMarkSlot[i] = TextButton::create(180, 45, "방장/준비완료", 15);
		m_pMarkSlot[i]->setAnchorPoint(Vec2::ZERO);
		m_pMarkSlot[i]->setBackgroundColor(ColorList::BlackShadows_v);
		m_pMarkSlot[i]->setFontColor(ColorList::Black_v);
		m_pMarkSlot[i]->setVisible(false);
		m_pUILayer->addChild(m_pMarkSlot[i]);
	}

	m_pSlot[0]->setPosition({ 210, 310 });
	m_pSlot[1]->setPosition({ 410, 310 });
	m_pSlot[2]->setPosition({ 210, 110 });
	m_pSlot[3]->setPosition({ 410, 110 });

	m_pMarkSlot[0]->setPosition({ 210, 310 + 135 });
	m_pMarkSlot[1]->setPosition({ 410, 310 + 135 });
	m_pMarkSlot[2]->setPosition({ 210, 110 + 135 });
	m_pMarkSlot[3]->setPosition({ 410, 110 + 135 });

	m_pRoomTitle = TextButton::create(200, 50, "방 제목 (1/4)", 16);
	m_pRoomTitle->setPosition({ 500, 550 });
	m_pRoomTitle->setBackgroundColor(ColorList::Caribbeangreen_v);
	m_pRoomTitle->setFontColor(ColorList::Black_v);
	m_pUILayer->addChild(m_pRoomTitle);

	return true;
}


void RoomScene::onClickedGameStartButton(TextButton* btn) {
	if (m_RoomInfo.IsBattleEndingState()) {
		PopUp::createInParent("게임이 거의 끝나갑니다. 기다려주세요!!", this, false);
		return;
	}

	if (m_RoomInfo.IsBattlePlayingState()) {
		PopUp::createInParent("이미 게임이 진행중입니다. 난입하시겠습니까?", this, false,
			[]() { S_GAME::SEND_CS_RoomGameIntrude(); },				// 수락
			[]() {});													// 거절
		return;
	}

	if (Core::GameClient->GetCharacterPrimaryKey() != m_iHostCharacterPrimaryKey) {
		PopUp::createInParent("방장만 게임을 시작할 수 있습니다.", this, false);
		return;
	}

	if (m_RoomInfo.PlayerCount == 0) {

		// 방 정보를 못받은 경우 로비로 이동
		PopUp::createInParent("방 정보를 받지 못했습니다. 다시 접속해주세요.", this, false);
		return;
	}

	int iReadyCount = 0;

	for (int i = 0; i < m_RoomInfo.PlayerCount; i++) {
		if (m_RoomMember[i].PrimaryKey == m_iHostCharacterPrimaryKey)
			continue;

		if (m_RoomMember[i].Ready)
			iReadyCount++;
	}

	if (iReadyCount == m_RoomInfo.PlayerCount - 1) {
		S_GAME::SEND_CS_RoomGameStart();
	}
}

void RoomScene::onClickedGameReadyButton(TextButton* btn) {
	if (Core::GameClient->GetCharacterPrimaryKey() == m_iHostCharacterPrimaryKey) {
		PopUp::createInParent("방장은 게임 준비를 할 수 없습니다.", this, false);
		return;
	}

	S_GAME::SEND_CS_RoomGameReady(true);
}

void RoomScene::onClickedGameReadyCancelButton(TextButton* btn) {
	// 방장 여부 체크 후 방장이 아니면 게임준비해제 패킷 전송
	if (Core::GameClient->GetCharacterPrimaryKey() == m_iHostCharacterPrimaryKey) {
		PopUp::createInParent("방장은 게임 준비를 할 수 없습니다.", this, false);
		return;
	}

	S_GAME::SEND_CS_RoomGameReady(false);
}

void RoomScene::onClickedGameLeaveRoomButton(TextButton* btn) {
	// 방 나가기 패킷 서버로 전송
	S_GAME::SEND_CS_RoomLeave();
}


void RoomScene::refreshRoomInfo(RoomInfo& info) {
	m_RoomInfo = info;
	m_pRoomTitle->setText(StringUtils::format("[%d] %s (%d/%d)", m_RoomInfo.AccessId - Const::AccessibleObject::StartId::Room, m_RoomInfo.Name.Source, m_RoomInfo.PlayerCount, m_RoomInfo.MaxPlayerCount));

	if (m_RoomInfo.IsBattlePlayingState()) {
		m_pGameStartBtn->setText("게임 난입");
	} else if (m_RoomInfo.IsLobbyState()) {
		m_pGameStartBtn->setText("게임 시작");
	}
}

void RoomScene::refreshRoomMemberInfoList(RoomCharacterInfo* roomCharacterInfoList, int count, int hostCharacterPrimaryKey) {
	const int iMyCharacterPrimaryKey = Core::GameClient->GetCharacterPrimaryKey();

	for (int i = 0; i < Const::Room::MaxPlayerCount; i++) {
		m_pSlot[i]->setVisible(false);
		m_pMarkSlot[i]->setVisible(false);
	}

	if (count > Const::Room::MaxPlayerCount) {
		DebugAssertMsg(false, "ROOM_MAX_PLAYER_COUNT 수치 초과하는 Count를 받음");
		count = Const::Room::MaxPlayerCount;
	}

	m_iHostCharacterPrimaryKey = hostCharacterPrimaryKey;

	for (int i = 0; i < count; i++) {

		m_pSlot[i]->setVisible(true);
		m_pMarkSlot[i]->setVisible(true);

		RoomCharacterInfo& info = roomCharacterInfoList[i];
		m_RoomMember[i] = info;

		refreshRoomMemberReadyState(i);
		refreshRoomMemberInfo(i, &info);
	}
}

void RoomScene::refreshRoomMemberReadyState(int memberIndex) {
	DebugAssert(memberIndex >= 0 && memberIndex < Const::Room::MaxPlayerCount);

	RoomCharacterInfo& info = m_RoomMember[memberIndex];

	const int iMyCharacterPrimaryKey = Core::GameClient->GetCharacterPrimaryKey();

	if (info.PrimaryKey != m_iHostCharacterPrimaryKey) {
		if (iMyCharacterPrimaryKey == info.PrimaryKey)
			m_pMarkSlot[memberIndex]->setText(info.Ready ? "(나) 준비 완료" : "(나)");
		else
			m_pMarkSlot[memberIndex]->setText(info.Ready ? "준비 완료" : "");
	} else {
		if (iMyCharacterPrimaryKey == info.PrimaryKey)
			m_pMarkSlot[memberIndex]->setText("(나) 방장");
		else
			m_pMarkSlot[memberIndex]->setText("방장");
	}
}

void RoomScene::refreshRoomMemberInfo(int memberIndex, RoomCharacterInfo* info) {
	DebugAssert(memberIndex >= 0 && memberIndex < Const::Room::MaxPlayerCount);
	m_pSlot[memberIndex]->setText(StringUtils::format("%s\n%d킬 %d데스\n%d승리 %d패배\n%d 골드",
		info->Name.Source,
		info->Kill,
		info->Death,
		info->Win,
		info->Lose,
		info->Money
	));
}

void RoomScene::updateRoomMemberReadyState(int characterPrimaryKey, bool ready) {
	const int iMemberIndex = getRoomMemberIndexByPrimaryKey(characterPrimaryKey);
	if (iMemberIndex == Const::InvalidValue) {
		return;
	}

	m_RoomMember[iMemberIndex].Ready = ready;
	refreshRoomMemberReadyState(iMemberIndex);
}


RoomCharacterInfo* RoomScene::getRoomMemberByPrimaryKey(int characterPrimaryKey) {
	int iIndex = Arrays::FindIf(m_RoomMember, [=](RoomCharacterInfo& info) { return info.PrimaryKey == characterPrimaryKey; });
	if (iIndex == Const::InvalidValue) return nullptr;
	return &m_RoomMember[iIndex];
}

int RoomScene::getRoomMemberIndexByPrimaryKey(int characterPrimaryKey) {
	return Arrays::FindIf(m_RoomMember, [=](RoomCharacterInfo& info) { return info.PrimaryKey == characterPrimaryKey; });
}
