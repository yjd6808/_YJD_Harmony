/*
 * �ۼ��� : ������
 */

#include <TF/Scenes/RoomLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>

#include <JCore/Ascii.h>

void RoomLayer::onEnterTransitionDidFinish() {
	// �濡 ������ �������� ��û�Ѵ�.
	if (SendFn::SendLoadRoomSyn() == false) {
		PopUp::createInParent("�� ���� ��û�� �����Ͽ����ϴ�.", this, false);
	}
}


bool RoomLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	// 700 350 - ���� ����
	// 700 300 - ���� �غ�
	// 700 250 - �غ� ����
	// 700 200 - �� ������

	m_pGameStartBtn = TextButton::create(200, 45, "���� ����", 15);
	m_pGameStartBtn->setAnchorPoint(Vec2::ZERO);
	m_pGameStartBtn->setPosition({ 700, 350 });
	m_pGameStartBtn->setBackgroundColor(ColorList::Babypink_v);
	m_pGameStartBtn->setFontColor(ColorList::Blackbean_v);
	m_pGameStartBtn->setClickEvent(CC_CALLBACK_1(RoomLayer::OnClickedGameStartButton, this));
	this->addChild(m_pGameStartBtn);

	m_pGameReadyBtn = TextButton::create(200, 45, "���� �غ�", 15);
	m_pGameReadyBtn->setAnchorPoint(Vec2::ZERO);
	m_pGameReadyBtn->setPosition({ 700, 300 });
	m_pGameReadyBtn->setBackgroundColor(ColorList::Ashgray_v);
	m_pGameReadyBtn->setFontColor(ColorList::Bistre_v);
	m_pGameReadyBtn->setClickEvent(CC_CALLBACK_1(RoomLayer::OnClickedGameReadyButton, this));
	this->addChild(m_pGameReadyBtn);

	m_pGameReadyCancelBtn = TextButton::create(200, 45, "�غ� ����", 15);
	m_pGameReadyCancelBtn->setAnchorPoint(Vec2::ZERO);
	m_pGameReadyCancelBtn->setPosition({ 700, 250 });
	m_pGameReadyCancelBtn->setBackgroundColor(ColorList::Ashgray_v);
	m_pGameReadyCancelBtn->setFontColor(ColorList::Bistre_v);
	m_pGameReadyCancelBtn->setClickEvent(CC_CALLBACK_1(RoomLayer::OnClickedGameReadyCancelButton, this));
	this->addChild(m_pGameReadyCancelBtn);

	m_pLeaveRoomBtn = TextButton::create(200, 45, "�� ������", 15);
	m_pLeaveRoomBtn->setAnchorPoint(Vec2::ZERO);
	m_pLeaveRoomBtn->setPosition({ 700, 200 });
	m_pLeaveRoomBtn->setBackgroundColor(ColorList::Ashgray_v);
	m_pLeaveRoomBtn->setFontColor(ColorList::Bistre_v);
	m_pLeaveRoomBtn->setClickEvent(CC_CALLBACK_1(RoomLayer::OnClickedGameLeaveRoomButton, this));
	this->addChild(m_pLeaveRoomBtn);

	for (int i = 0; i < ROOM_MAX_PLAYER_COUNT; i++) {
		m_pSlot[i] = TextButton::create(180, 135, "�г���", 15);
		m_pSlot[i]->setAnchorPoint(Vec2::ZERO);
		m_pSlot[i]->setBackgroundColor(ColorList::Ashgray_v);
		m_pSlot[i]->setFontColor(ColorList::Bistre_v);
		m_pSlot[i]->setVisible(false);
		this->addChild(m_pSlot[i]);

		m_pMarkSlot[i] = TextButton::create(180, 45, "����/�غ�Ϸ�", 15);
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

	m_pRoomTitle = TextButton::create(200, 50, "�� ���� (1/4)", 16);
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
	// ���� ���� üũ �� �����̸� ������ ���ӽ��� ��Ŷ ����
	if (m_iRoomMemberCount == 0) {

		// �� ������ ������ ��� �κ�� �̵�
		PopUp::createInParent("�� ������ ���� ���߽��ϴ�. �ٽ� �������ּ���.", this, false);
		return;
	}

	/*
	// ȥ�ڼ��� �����ϵ��� ��
	if (m_iRoomMemberCount == 1) {
		PopUp::createInParent("ȥ�ڼ��� ������ �� �����ϴ�.", this, false);
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
		PopUp::createInParent("�ڽ��� ������ �� �� �����ϴ�. �ٽ� �������ּ���.", this, false);
		return;
	}

	if (pMyInfo->CharacterUID == m_iHostCharacterUID) {
		PopUp::createInParent("������ ���� �غ� �� �� �����ϴ�.", this, false);
		return;
	}

	SendFn::SendRoomGameReadySyn();
}

void RoomLayer::OnClickedGameReadyCancelButton(TextButton* btn) {
	// ���� ���� üũ �� ������ �ƴϸ� �����غ����� ��Ŷ ����
	RoomCharacterInfo* pMyInfo = MyInfo();

	if (pMyInfo == nullptr) {
		PopUp::createInParent("�ڽ��� ������ �� �� �����ϴ�. �ٽ� �������ּ���.", this, false);
		return;
	}

	if (pMyInfo->CharacterUID == m_iHostCharacterUID) {
		PopUp::createInParent("������ ���� �غ� �� �� �����ϴ�.", this, false);
		return;
	}

	SendFn::SendRoomGameReadyCancelSyn();
}

void RoomLayer::OnClickedGameLeaveRoomButton(TextButton* btn) {
	// �� ������ ��Ŷ ������ ����
	SendFn::SendRoomLeaveSyn();
}


/* =================================================================================
 *
 *                             ��� ��Ŷ ó��
 *
 * ================================================================================*/


void RoomLayer::CmdLoadRoomInfoAck(ICommand* cmd) {
	LoadRoomInfoAck* pLoadRoomInfoAck = cmd->CastCommand<LoadRoomInfoAck*>();

	if (pLoadRoomInfoAck->Result) {
		Memory::CopyUnsafe(&m_RoomInfo, &pLoadRoomInfoAck->Info, sizeof(RoomInfo));
		m_pRoomTitle->setText(StringUtils::format("[%d] %s (%d/%d)", m_RoomInfo.RoomUID, m_RoomInfo.Name, m_RoomInfo.PlayerCount, m_RoomInfo.MaxPlayerCount));
		return;
	}

	// �� ������ ������ ��� �κ�� �̵�
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
		DebugAssert(false, "ROOM_MAX_PLAYER_COUNT ��ġ �ʰ��ϴ� Count�� ����");
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
				m_pMarkSlot[i]->setText(m_RoomMember[i].Ready ? "(��) �غ� �Ϸ�" : "(��)");
			else
				m_pMarkSlot[i]->setText(m_RoomMember[i].Ready ? "�غ� �Ϸ�" : "");
		} else {
			if (_Client->GetCharacterUID() == pRoomCharacterInfo->CharacterUID)
				m_pMarkSlot[i]->setText("(��) ����");
			else
				m_pMarkSlot[i]->setText("����");
		}

		m_pSlot[i]->setText(StringUtils::format("%s\n%dų %d����\n%d�¸� %d�й�\n%d ���", 
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
