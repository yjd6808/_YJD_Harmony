/*
 * 작성자 : 윤정도
 */

#include <TF/Scenes/RoomLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>

#include <JCore/Ascii.h>

void RoomLayer::onEnterTransitionDidFinish() {
	// 방에 들어오면 방정보를 요청한다.
	if (SendFn::SendLoadRoomSyn() == false) {
		PopUp::createInParent("방 정보 요청이 실패하였습니다.", this, false);
	}
}


bool RoomLayer::init() {
	if (!Layer::init()) {
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
	m_pGameStartBtn->setClickEvent(CC_CALLBACK_1(RoomLayer::OnClickedGameStartButton, this));
	this->addChild(m_pGameStartBtn);

	m_pGameReadyBtn = TextButton::create(200, 45, "시작 준비", 15);
	m_pGameReadyBtn->setAnchorPoint(Vec2::ZERO);
	m_pGameReadyBtn->setPosition({ 700, 300 });
	m_pGameReadyBtn->setBackgroundColor(ColorList::Ashgray_v);
	m_pGameReadyBtn->setFontColor(ColorList::Bistre_v);
	m_pGameReadyBtn->setClickEvent(CC_CALLBACK_1(RoomLayer::OnClickedGameReadyButton, this));
	this->addChild(m_pGameReadyBtn);

	m_pGameReadyCancelBtn = TextButton::create(200, 45, "준비 해제", 15);
	m_pGameReadyCancelBtn->setAnchorPoint(Vec2::ZERO);
	m_pGameReadyCancelBtn->setPosition({ 700, 250 });
	m_pGameReadyCancelBtn->setBackgroundColor(ColorList::Ashgray_v);
	m_pGameReadyCancelBtn->setFontColor(ColorList::Bistre_v);
	m_pGameReadyCancelBtn->setClickEvent(CC_CALLBACK_1(RoomLayer::OnClickedGameReadyCancelButton, this));
	this->addChild(m_pGameReadyCancelBtn);

	m_pLeaveRoomBtn = TextButton::create(200, 45, "방 나가기", 15);
	m_pLeaveRoomBtn->setAnchorPoint(Vec2::ZERO);
	m_pLeaveRoomBtn->setPosition({ 700, 200 });
	m_pLeaveRoomBtn->setBackgroundColor(ColorList::Ashgray_v);
	m_pLeaveRoomBtn->setFontColor(ColorList::Bistre_v);
	m_pLeaveRoomBtn->setClickEvent(CC_CALLBACK_1(RoomLayer::OnClickedGameLeaveRoomButton, this));
	this->addChild(m_pLeaveRoomBtn);

	for (int i = 0; i < ROOM_MAX_PLAYER_COUNT; i++) {
		m_pSlot[i] = TextButton::create(180, 135, "닉네임", 15);
		m_pSlot[i]->setAnchorPoint(Vec2::ZERO);
		m_pSlot[i]->setBackgroundColor(ColorList::Ashgray_v);
		m_pSlot[i]->setFontColor(ColorList::Bistre_v);
		m_pSlot[i]->setVisible(false);
		this->addChild(m_pSlot[i]);

		m_pMarkSlot[i] = TextButton::create(180, 45, "방장/준비완료", 15);
		m_pMarkSlot[i]->setAnchorPoint(Vec2::ZERO);
		m_pMarkSlot[i]->setBackgroundColor(ColorList::BlackShadows_v);
		m_pMarkSlot[i]->setFontColor(ColorList::Bluegreen_v);
		m_pMarkSlot[i]->setVisible(false);
		this->addChild(m_pMarkSlot[i]);
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
	this->addChild(m_pRoomTitle);
	return true;
}


RoomCharacterInfo* RoomLayer::MyInfo() {
	for (int i = 0; i < m_iRoomMemberCount; i++) {
		if (m_RoomMember[i].CharacterUID == _Client->GetCharacterUID())
			return &m_RoomMember[i];
	}

	return nullptr;
}

void RoomLayer::OnClickedGameStartButton(TextButton* btn) {
	// 방장 여부 체크 후 방장이면 서버에 게임시작 패킷 전송
	if (m_iRoomMemberCount == 0) {

		// 방 정보를 못받은 경우 로비로 이동
		PopUp::createInParent("방 정보를 받지 못했습니다. 다시 접속해주세요.", this, false);
		return;
	}

	/*
	// 혼자서도 가능하도록 함
	if (m_iRoomMemberCount == 1) {
		PopUp::createInParent("혼자서는 시작할 수 없습니다.", this, false);
		return;
	}
	*/

	int iReadyCount = 0;

	for (int i = 0 ; i < m_iRoomMemberCount; i++) {
		if (m_RoomMember[i].CharacterUID == m_iHostCharacterUID)
			continue;

		if (m_RoomMember[i].Ready)
			iReadyCount++;
	}

	if (iReadyCount == m_iRoomMemberCount - 1) {
		SendFn::SendRoomGameStartSyn();
	}
}

void RoomLayer::OnClickedGameReadyButton(TextButton* btn) {
	RoomCharacterInfo* pMyInfo = MyInfo();

	if (pMyInfo == nullptr) {
		PopUp::createInParent("자신이 누군지 알 수 없습니다. 다시 접속해주세요.", this, false);
		return;
	}

	if (pMyInfo->CharacterUID == m_iHostCharacterUID) {
		PopUp::createInParent("방장은 게임 준비를 할 수 없습니다.", this, false);
		return;
	}

	SendFn::SendRoomGameReadySyn();
}

void RoomLayer::OnClickedGameReadyCancelButton(TextButton* btn) {
	// 방장 여부 체크 후 방장이 아니면 게임준비해제 패킷 전송
	RoomCharacterInfo* pMyInfo = MyInfo();

	if (pMyInfo == nullptr) {
		PopUp::createInParent("자신이 누군지 알 수 없습니다. 다시 접속해주세요.", this, false);
		return;
	}

	if (pMyInfo->CharacterUID == m_iHostCharacterUID) {
		PopUp::createInParent("방장은 게임 준비를 할 수 없습니다.", this, false);
		return;
	}

	SendFn::SendRoomGameReadyCancelSyn();
}

void RoomLayer::OnClickedGameLeaveRoomButton(TextButton* btn) {
	// 방 나가기 패킷 서버로 전송
	SendFn::SendRoomLeaveSyn();
}


/* =================================================================================
 *
 *                             통신 패킷 처리
 *
 * ================================================================================*/


void RoomLayer::CmdLoadRoomInfoAck(ICommand* cmd) {
	LoadRoomInfoAck* pLoadRoomInfoAck = cmd->CastCommand<LoadRoomInfoAck*>();

	if (pLoadRoomInfoAck->Result) {
		Memory::CopyUnsafe(&m_RoomInfo, &pLoadRoomInfoAck->Info, sizeof(RoomInfo));
		m_pRoomTitle->setText(StringUtils::format("[%d] %s (%d/%d)", m_RoomInfo.RoomUID, m_RoomInfo.Name, m_RoomInfo.PlayerCount, m_RoomInfo.MaxPlayerCount));
		return;
	}

	// 방 정보를 못받은 경우 로비로 이동
	_Client->SetRoomUID(INVALID_UID);
	_Client->ChangeScene(SceneType::Lobby);
}

void RoomLayer::CmdUpdateRoomInfoAck(ICommand* cmd) {
	for (int i = 0; i < ROOM_MAX_PLAYER_COUNT; i++) {
		m_pSlot[i]->setVisible(false);
		m_pMarkSlot[i]->setVisible(false);
	}

	UpdateRoomInfoAck* pUpdateRoomInfoAck = cmd->CastCommand<UpdateRoomInfoAck*>();

	if (pUpdateRoomInfoAck->Count > ROOM_MAX_PLAYER_COUNT) {
		DebugAssert(false, "ROOM_MAX_PLAYER_COUNT 수치 초과하는 Count를 받음");
		pUpdateRoomInfoAck->Count = ROOM_MAX_PLAYER_COUNT;
	}

	m_iRoomMemberCount = pUpdateRoomInfoAck->Count;
	m_iHostCharacterUID = pUpdateRoomInfoAck->HostCharacterUID;

	for (int i = 0 ; i < m_iRoomMemberCount; i++) {
		Memory::CopyUnsafe(&m_RoomMember[i], &pUpdateRoomInfoAck->Info[i], sizeof(RoomCharacterInfo));
	}

	for (int i = 0; i < m_iRoomMemberCount; i++) {

		m_pSlot[i]->setVisible(true);
		m_pMarkSlot[i]->setVisible(true);

		RoomCharacterInfo* pRoomCharacterInfo = &m_RoomMember[i];

		if (pRoomCharacterInfo->CharacterUID != m_iHostCharacterUID) {
			if (_Client->GetCharacterUID() == pRoomCharacterInfo->CharacterUID)
				m_pMarkSlot[i]->setText(m_RoomMember[i].Ready ? "(나) 준비 완료" : "(나)");
			else
				m_pMarkSlot[i]->setText(m_RoomMember[i].Ready ? "준비 완료" : "");
		} else {
			if (_Client->GetCharacterUID() == pRoomCharacterInfo->CharacterUID)
				m_pMarkSlot[i]->setText("(나) 방장");
			else
				m_pMarkSlot[i]->setText("방장");
		}

		m_pSlot[i]->setText(StringUtils::format("%s\n%d킬 %d데스\n%d승리 %d패배\n%d 골드", 
			m_RoomMember[i].Name,
			m_RoomMember[i].Kill,
			m_RoomMember[i].Death,
			m_RoomMember[i].Win,
			m_RoomMember[i].Lose,
			m_RoomMember[i].Money
		));
	}
}

void RoomLayer::CmdRoomLeaveAck(ICommand* cmd) {
	_Client->SetRoomUID(INVALID_UID);
	_Client->ChangeScene(SceneType::Lobby);
}

void RoomLayer::CmdRoomGameStartAck(ICommand* cmd) {
	RoomGameStartAck* pRoomGameStartAck = cmd->CastCommand<RoomGameStartAck*>();

	if (!pRoomGameStartAck->Result) {
		PopUp::createInParent(pRoomGameStartAck->Reason, this, false);
		return;
	}

	_Client->ChangeScene(SceneType::BattleField);
}
