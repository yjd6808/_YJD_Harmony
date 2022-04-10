/*
 * �ۼ��� : ������
 */


#include <TF/Scenes/BattleFieldLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>
#include <JCore/Ascii.h>

#include "TF/Object/Bullet.h"

const char* BattleFieldLayer::ms_pPlayWaitLeftTimeFormat = "�ٸ� �÷��̾ ��� �����Ҷ����� ��ٷ��ּ���.\n�����ð� : %.1f��";
const char* BattleFieldLayer::ms_pPlayingLeftTimeFormat = "���� ������� �����ð� : %.1f��";
const char* BattleFieldLayer::ms_pEndWaitLeftTimeFormat = "������ �̵��ϱ���� �����ð� : % .1f��";

void BattleFieldLayer::onEnterTransitionDidFinish() {
	// ��Ʋ�ʵ忡 ������ 
	if (!SendFn::SendBattleFieldLoadSyn()) {
		PopUp::createInParent("��Ʋ�ʵ� ���� ��û�� �����Ͽ����ϴ�.", this, false);
	}
}



bool BattleFieldLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	// ��ũ�� �ƹ��͵� ���ϴ� �̸� �������´�.
	m_pTank = Tank::create(_Client->GetCharacterUID(), this);
	m_pTank->setVisible(false);
	m_pTank->SetFireCallback(CC_CALLBACK_1(BattleFieldLayer::OnFireTank, this));
	this->addChild(m_pTank, 2);

	m_pTimeText = Text::create(ms_pPlayWaitLeftTimeFormat, FONT_PATH_DEFAULT, 25);
	m_pTimeText->setColor(ColorList::Airsuperiorityblue_v);
	m_pTimeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pTimeText->setVisible(false);
	m_pTimeText->setPosition({ 500, 400 });
	this->addChild(m_pTimeText);

	m_pRevivalText = Text::create(ms_pPlayWaitLeftTimeFormat, FONT_PATH_DEFAULT, 20);
	m_pRevivalText->setColor(ColorList::Africanviolet_v);
	m_pRevivalText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pRevivalText->setVisible(false);
	m_pRevivalText->setPosition({ 500.0f, 250.0f });
	this->addChild(m_pRevivalText);


	m_pNoticeText = Text::create("", FONT_PATH_DEFAULT, 25);
	m_pNoticeText->setColor(ColorList::Arylideyellow_v);
	m_pNoticeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pNoticeText->setOpacity(0);
	m_pNoticeText->setPosition({ 500, 450 });
	this->addChild(m_pNoticeText);
	
	// �ڱ� �ڽ��� ������ �ִ� ����ŭ �̸� ��ũ�� �����س��´�.
	for (int i = 0; i < ROOM_MAX_PLAYER_COUNT - 1; i++) {
		Tank* pTank = Tank::create(INVALID_UID, this);
		pTank->setVisible(false);
		this->addChild(pTank, 2);
		m_OtherPlayers.PushBack(pTank);
	}

	m_pChatBox = ChatBox::create(ColorList::BlackShadows_v, { 400, 200 });
	m_pChatBox->setAnchorPoint(Vec2::ZERO);
	m_pChatBox->SetSendButtonClickEvent(CC_CALLBACK_1(BattleFieldLayer::OnClickedChatSendButton, this));
	m_pChatBox->SetContentSize({ 400, 200 });
	this->addChild(m_pChatBox);

	m_pClickPrevenButton = Button::create(RECT_IMG_FILENAME, RECT_IMG_FILENAME, RECT_IMG_FILENAME);
	m_pClickPrevenButton->setScale9Enabled(true);
	m_pClickPrevenButton->setColor(Color3B::GRAY);
	m_pClickPrevenButton->setOpacity(60);
	m_pClickPrevenButton->setEnabled(false);			// �̷��� �θ� Ŭ�� ����
	m_pClickPrevenButton->setVisible(false);			// ó�� �Ⱥ��̵���
	m_pClickPrevenButton->setContentSize({ MAP_WIDTH, MAP_HEIGHT });
	m_pClickPrevenButton->setSize({ MAP_WIDTH, MAP_HEIGHT });
	m_pClickPrevenButton->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pClickPrevenButton, 100);



	m_pLeaveChannelButton = TextButton::create(200, 50, "���� ������", 16);
	m_pLeaveChannelButton->setBackgroundColor(ColorList::Bluegray_v);
	m_pLeaveChannelButton->setFontColor(ColorList::Black_v);
	m_pLeaveChannelButton->setAnchorPoint(Vec2::ZERO);
	m_pLeaveChannelButton->setPosition({ 800, 0 });
	m_pLeaveChannelButton->setClickEvent(CC_CALLBACK_1(BattleFieldLayer::OnClickedLeaveGameButton, this));
	this->addChild(m_pLeaveChannelButton);

	for (int i = 0; i < 4; i++) {
		m_pNameText[i] = Text::create("������ ����", FONT_PATH_DEFAULT, 14);
		m_pNameText[i]->setColor(ColorList::Blizzardblue_v);
		m_pNameText[i]->setAnchorPoint(Vec2::ZERO);
		m_pNameText[i]->setContentSize({ 100.0f, 0.0 });
		m_pNameText[i]->setVisible(false);
		m_pNameText[i]->setPosition({ 0, 400.0f - i * 20.0f });
		this->addChild(m_pNameText[i], 100);

		m_pKillText[i] = Text::create("0ų", FONT_PATH_DEFAULT, 14);
		m_pKillText[i]->setColor(ColorList::Amber_v);
		m_pKillText[i]->setAnchorPoint(Vec2::ZERO);
		m_pKillText[i]->setVisible(false);
		m_pKillText[i]->setPosition({ m_pNameText[i]->getPosition().x + m_pNameText[i]->getContentSize().width + 20.0f, 400.0f - i * 20.0f });
		this->addChild(m_pKillText[i], 100);

		m_pDeathText[i] = Text::create("0����", FONT_PATH_DEFAULT, 14);
		m_pDeathText[i]->setColor(ColorList::Alabaster_v);
		m_pDeathText[i]->setAnchorPoint(Vec2::ZERO);
		m_pDeathText[i]->setVisible(false);
		m_pDeathText[i]->setPosition({ m_pKillText[i]->getPosition().x + m_pKillText[i]->getContentSize().width + 20.0f, 400.0f - i * 20.0f});
		this->addChild(m_pDeathText[i], 100);
	}

	this->scheduleUpdate();
	return true;
}

void BattleFieldLayer::update(float delta) {
	// ====================================================== ��ũ�� ��ġ ������Ʈ
	m_fSendTankMoveSynTime += delta;

	m_pTank->updatePosition(delta);
	m_pTank->updateRotation(delta);
	m_pTank->updateShotDelay(delta);

	Vec2 myPos = m_pTank->getPosition();

	// �� ������ �����°� ����
	if (myPos.x <= 0) {
		m_pTank->setPositionX(1);
	} else if (myPos.x >= MAP_WIDTH) {
		m_pTank->setPositionX(MAP_WIDTH - 1);
	}

	if (myPos.y <= 0) {
		m_pTank->setPositionY(1);
	} else if (myPos.y >= MAP_HEIGHT) {
		m_pTank->setPositionY(MAP_HEIGHT - 1);
	}

	// ���� �ֱ⸶�� ������ ��ũ ������ ����������. ���׾��ִ� ���
	if (m_fSendTankMoveSynTime >= ms_iPlayerSendMoveStatePacketDelay && !IsDeath(m_pTank->GetCharacterUID())) {
		SendFn::SendBattileFieldTankMoveSyn(m_pTank->GetMove());
		m_fSendTankMoveSynTime = 0.0f;
	}

	for (int i = 0; i < m_OtherPlayers.Size(); i++) {
		if (m_OtherPlayers[i]->HasOwner()) {
			m_OtherPlayers[i]->updatePosition(delta);
			m_OtherPlayers[i]->updateRotation(delta);
		}
	}

	// ====================================================== �����ð� �ؽ�Ʈ ������Ʈ
	m_fLeftTime -= delta;
	if (m_fLeftTime >= 0) {
		m_pTimeText->setVisible(true);
		switch (m_eRoomState) {
		case RoomState::PlayWait: m_pTimeText->setText(StringUtils::format(ms_pPlayWaitLeftTimeFormat, m_fLeftTime));	break;
		case RoomState::Playing: m_pTimeText->setText(StringUtils::format(ms_pPlayingLeftTimeFormat, m_fLeftTime));		break;
		case RoomState::EndWait: m_pTimeText->setText(StringUtils::format(ms_pEndWaitLeftTimeFormat, m_fLeftTime));		break;
		}
	} else {
		m_pTimeText->setVisible(false);
	}


	// ====================================================== ��Ȱ�ð� ������Ʈ
	m_fRevivalLeftTime -= delta;

	if (m_fRevivalLeftTime >= 0.0f) {
		m_pRevivalText->setVisible(true);
		m_pRevivalText->setText(StringUtils::format("%.1f�ʵ� ��Ȱ�մϴ�.", m_fRevivalLeftTime));
	} else {
		m_pRevivalText->setVisible(false);
	}


	// ====================================================== �Ѿ� �浹 üũ


	for (int i = 0; i < m_MyBullets.Size(); ) {
		Bullet* pBullet = m_MyBullets[i];

		if (pBullet->getPosition().x < 0 || pBullet->getPosition().x > MAP_WIDTH ||
			pBullet->getPosition().y < 0 || pBullet->getPosition().y > MAP_HEIGHT) {
			m_MyBullets.Remove(pBullet);
			this->removeChild(pBullet);
		} else {
			i++;
		}
	}

	for (int i = 0; i < m_OtherBullets.Size(); i++) {
		Bullet* pBullet = m_OtherBullets[i];

		// �Ѿ��� �� ������ ��� ��� ������Ų��.
		if (pBullet->getPosition().x < 0 || pBullet->getPosition().x > MAP_WIDTH ||
			pBullet->getPosition().y < 0 || pBullet->getPosition().y > MAP_HEIGHT) {

			m_OtherBullets.Remove(pBullet);
			this->removeChild(pBullet);
			i--;
			continue;
		}

		if (m_pTank->IsCollide(pBullet) && !IsDeath(m_pTank->GetCharacterUID())) {

			// �� ��ũ�� ���� ��Ʈ�� ���ϰ� �Ѵ�.
			m_pTank->setVisible(false);
			m_pTank->setMoveable(false);
			m_pTank->setFireable(false);
			SetDeath(m_pTank->GetCharacterUID(), true);
			
			SendFn::SendBattleFieldDeathSyn(pBullet->GetCharacterUID());


			// �Ѿ��� �����Ѵ�.
			m_OtherBullets.Remove(pBullet);
			this->removeChild(pBullet);
			i--;
			continue;
		}

		for (int j = 0; j < m_OtherPlayers.Size(); j++) {
			Tank* pOther = m_OtherPlayers[j];

			// �ٸ� ��ũ�� �΋H�� ��쿡�� �Ѿ˸� ���ش�.
			if (pOther->IsCollide(pBullet) && !IsDeath(pOther->GetCharacterUID()) ) {
				m_OtherBullets.Remove(pBullet);
				this->removeChild(pBullet);
				i--;
				break;
			}
		}
	}
}

void BattleFieldLayer::OnFireTank(Bullet* bullet) {
	BulletInfo info;
	bullet->LoadBulletInfo(info);
	SendFn::SendBattleFieldFireSyn(info);
	m_MyBullets.PushBack(bullet);
}

void BattleFieldLayer::UpdateTankMove(TankMove& move)  {
	if (move.CharacterUID == INVALID_UID)
		return;

	if (move.CharacterUID == _Client->GetCharacterUID()) {
		return;
	}

	for (int i = 0; i < m_OtherPlayers.Size(); i++) {
		Tank* pOtherTank = m_OtherPlayers[i];

		// �����༮�� �ǳʶ�
		if (IsDeath(pOtherTank->GetCharacterUID())) {
			continue;
		}

		if (!pOtherTank->HasOwner() || pOtherTank->GetCharacterUID() == move.CharacterUID) {
			pOtherTank->SetCharacterUID(move.CharacterUID);
			pOtherTank->UpdateTankMove(move);
			pOtherTank->setVisible(true);
			return;
		} 
	}
}


void BattleFieldLayer::OnClickedLeaveGameButton(TextButton* btn) {
	PopUp::createInParent("������ Ż���Ͻðڽ��ϱ�?", this, false,
		[]() {
			// ������ �κ�� �̵���������.
			SendFn::SendBattleFieldLeaveSyn();
			_Client->SetRoomUID(INVALID_UID);
			_Client->ChangeScene(SceneType::Lobby);
		},	 
		[]() { }); // ���� 
}

JCore::String BattleFieldLayer::GetRoomMemberName(int characterUID) const {
	for (int i = 0; i < m_iRoomMemberCount; i++) {
		if (m_RoomMember[i].CharacterUID == characterUID) {
			return m_RoomMember[i].Name;
		}
	}
	return "???";
}

bool BattleFieldLayer::IsDeath(int characterUID) {
	if (characterUID == INVALID_UID) {
		return false;
	}

	for (int i = 0; i <m_iRoomMemberCount; i++) {
		if (m_RoomMember[i].IsDeath && m_RoomMember[i].CharacterUID == characterUID) {
			return true;
		}
	}

	return false;
}

void BattleFieldLayer::SetDeath(int characterUID, bool isDeath) {
	if (characterUID == INVALID_UID)
		return;

	for (int i = 0; i < m_iRoomMemberCount; i++) {
		if (m_RoomMember[i].CharacterUID == characterUID) {
			m_RoomMember[i].IsDeath = isDeath;
			return;
		}
	}
}

void BattleFieldLayer::UpdateUIModeByRoomState() {
	switch (m_eRoomState) {
	case RoomState::PlayWait: {
		m_pTimeText->setColor(ColorList::Airsuperiorityblue_v);
		m_pTimeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pTimeText->setFontSize(25);
		m_pTimeText->setPosition({ 500, 400 });
		break;
	}
	case RoomState::Playing: {
		m_pTimeText->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		m_pTimeText->setPosition(Vec2(MAP_WIDTH, MAP_HEIGHT));
		m_pTimeText->setFontSize(14);
		m_pTimeText->setColor(ColorList::Celeste_v);
		break;
	}
	case RoomState::EndWait: {
		m_pTimeText->setColor(ColorList::Airsuperiorityblue_v);
		m_pTimeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pTimeText->setFontSize(25);
		m_pTimeText->setPosition({ 500, 400 });
		break;
	}
	}

}

void BattleFieldLayer::OnClickedChatSendButton(ChatBox* chatBox) {
	JCore::String message =  chatBox->GetEditBox()->getText();
	if (message.Length() == 0) {
		return;
	}
	SendFn::SendChatMessage(message);
}




/* =================================================================================
 *
 *                             ��� ��Ŷ ó��
 *
 * ================================================================================*/

// �� ��� ����� ���ڱ� ���� �����ų� �� ��� ������Ʈ �������
void BattleFieldLayer::CmdUpdateRoomInfoAck(ICommand* cmd) {
	UpdateRoomInfoAck* pUpdateRoomInfoAck = cmd->CastCommand<UpdateRoomInfoAck*>();

	if (pUpdateRoomInfoAck->Count > ROOM_MAX_PLAYER_COUNT) {
		DebugAssert(false, "ROOM_MAX_PLAYER_COUNT ��ġ �ʰ��ϴ� Count�� ����");
		pUpdateRoomInfoAck->Count = ROOM_MAX_PLAYER_COUNT;
	}

	m_iRoomMemberCount = pUpdateRoomInfoAck->Count;
	m_iHostCharacterUID = pUpdateRoomInfoAck->HostCharacterUID;

	for (int i = 0; i < m_iRoomMemberCount; i++) {
		Memory::CopyUnsafe(&m_RoomMember[i], &pUpdateRoomInfoAck->Info[i], sizeof(RoomCharacterInfo));
	}

	for (int i = 0; i < m_OtherPlayers.Size(); i++) {

		bool bExistIntRoom = false;
		for (int j = 0; j < m_iRoomMemberCount; j++) {
			if (m_OtherPlayers[i]->GetCharacterUID() == m_RoomMember[i].CharacterUID) {
				bExistIntRoom = true;
				break;
			}
		}

		if (!bExistIntRoom) {
			m_OtherPlayers[i]->setVisible(false);
			m_OtherPlayers[i]->setPosition({ -500.0f, -500.0f });
			m_OtherPlayers[i]->SetCharacterUID(INVALID_UID);
		}
	}
}

void BattleFieldLayer::CmdUpdateCharacterInfoAck(ICommand* cmd) {
	UpdateCharacterInfoAck* pUpdateCharacterInfoAck = cmd->CastCommand<UpdateCharacterInfoAck*>();

	// ĳ���� ���� ������Ʈ
	if (pUpdateCharacterInfoAck->Result) {
		_Client->UpdateCharacterInfo(pUpdateCharacterInfoAck->Info);
		return;
	}

	PopUp::createInParent(pUpdateCharacterInfoAck->Reason, this, false, [this]() {
		_Client->SetChannelUID(INVALID_UID);
		_Client->SetCharacterUID(INVALID_UID);
		_Client->SetRoomUID(INVALID_UID);
		_Client->ChangeScene(SceneType::Channel);
	});
}


void BattleFieldLayer::CmdBattleFieldLoadAck(ICommand* cmd) {
	BattleFieldLoadAck* pBattleFieldLoadAck = cmd->CastCommand<BattleFieldLoadAck*>();

	m_eRoomState = pBattleFieldLoadAck->RoomState;
	m_fLeftTime = pBattleFieldLoadAck->LeftTime;

	// �����Ͽ� ������ ��� ����¿� ���� m_pTimeText ��Ʈũ��� ��ġ�� �ٸ�
	UpdateUIModeByRoomState();

	m_pTank->setVisible(true);
	m_pTank->UpdateTankMove(pBattleFieldLoadAck->InitialMove);
	m_pTank->setMoveable(true);
	m_pTank->setFireable(true);
}

void BattleFieldLayer::CmdBattleFieldLeaveAck(ICommand* cmd) const {
	const BattleFieldLeaveAck* pBattleFieldLeaveAck = cmd->CastCommand<BattleFieldLeaveAck*>();

	for (int i = 0; i < m_OtherPlayers.Size(); i++) {
		if (m_OtherPlayers[i]->GetCharacterUID() == pBattleFieldLeaveAck->CharacterUID) {
			m_OtherPlayers[i]->setVisible(false);
			m_OtherPlayers[i]->SetCharacterUID(INVALID_UID);
		}
	}
}


void BattleFieldLayer::CmdBattleFieldTankUpdateSyn(ICommand* cmd) {
	BattileFieldTankUpdateSyn* pBattileFieldTankUpdateSyn = cmd->CastCommand<BattileFieldTankUpdateSyn*>();

	for (int i = 0; i < pBattileFieldTankUpdateSyn->Count; i++) {
		// ���� ����� ������Ʈ ����
		UpdateTankMove(pBattileFieldTankUpdateSyn->Move[i]);
	}

	// ������Ʈ �ȵ� �ֵ��� ���� �ֵ��̹Ƿ� �Ⱥ��̵��� ��
	for (int i = 0; i < m_OtherPlayers.Size(); i++) {
		if (!m_OtherPlayers[i]->HasOwner()) {
			m_OtherPlayers[i]->setVisible(false);
			m_OtherPlayers[i]->setPosition(-500, -500);
		}
	}
}

// ���׸°� �� ����� ������ ���� �÷����߿� �ٸ� ������ ���� ������ �� ��쿡 �� Ŀ�ǵ带 �����Ѵ�.
void BattleFieldLayer::CmdRoomGameStartAck(ICommand* cmd) const {
	RoomGameStartAck* pRoomGameStartAck = cmd->CastCommand<RoomGameStartAck*>();
	FadeOut* pFadeOutAnimation = FadeOut::create(5.0f);
	m_pNoticeText->setOpacity(255);
	m_pNoticeText->runAction(pFadeOutAnimation);
	

	const JCore::String message = StringUtil::Format("%s ���� �� ���� �� �ϼ̽��ϴ�.", pRoomGameStartAck->IntruderInfo.Name);
	m_pNoticeText->setText(message.Source());
	m_pChatBox->AddNoticeMessage(message);
}


void BattleFieldLayer::CmdBattleFieldPlayWaitEndSyn(ICommand* cmd) {
	const BattleFieldPlayWaitEndSyn* pBattleFieldPlayWaitEndSyn = cmd->CastCommand<BattleFieldPlayWaitEndSyn*>();
	FadeOut* pFadeOutAnimation = FadeOut::create(5.0f);
	m_pNoticeText->setOpacity(255);
	m_pNoticeText->runAction(pFadeOutAnimation);

	m_pNoticeText->setText("���� ����");
	m_pChatBox->AddNoticeMessage("���� ����");
	m_pTank->setFireable(true);
	m_eRoomState = pBattleFieldPlayWaitEndSyn->RoomState;
	m_fLeftTime = pBattleFieldPlayWaitEndSyn->LeftTime / 1000.0f;

	UpdateUIModeByRoomState();
}

void BattleFieldLayer::CmdBattleFieldPlayingEndSyn(ICommand* cmd) {
	const BattleFieldPlayingEndSyn* pBattleFieldPlayWaitEndSyn = cmd->CastCommand<BattleFieldPlayingEndSyn*>();
	FadeOut* pFadeOutAnimation = FadeOut::create(5.0f);
	m_pNoticeText->setOpacity(255);
	m_pNoticeText->runAction(pFadeOutAnimation);
	m_pNoticeText->setText("���� ���� - ��� �� �κ�� �̵��մϴ�.");
	m_pChatBox->AddNoticeMessage("���� ���� - ��� �� �κ�� �̵��մϴ�.");
	m_pTank->setFireable(false);
	m_eRoomState = pBattleFieldPlayWaitEndSyn->RoomState;
	m_fLeftTime = pBattleFieldPlayWaitEndSyn->LeftTime / 1000.0f;

	UpdateUIModeByRoomState();
}

void BattleFieldLayer::CmdBattleFieldEndWaitEndSyn(ICommand* cmd) {
	_Client->ChangeScene(SceneType::Room);
}

void BattleFieldLayer::CmdChatMessageAck(ICommand* cmd) const {
	const ChatMessageAck* pChatMessageAck = cmd->CastCommand<ChatMessageAck*>();
	m_pChatBox->AddChatMessage(pChatMessageAck->Message);
}

void BattleFieldLayer::CmdBattleFieldFireAck(ICommand* cmd) {
	BattleFieldFireAck* pBattleFieldFireAck = cmd->CastCommand<BattleFieldFireAck*>();

	// ���� �� �Ѿ��� ������Ʈ ����
	if (pBattleFieldFireAck->BulletInfo.CharacterUID == _Client->GetCharacterUID()) {
		return;
	}

	Bullet* pBullet = Bullet::create(0.0f, 0.0f);
	pBullet->UpdateBulletInfo(pBattleFieldFireAck->BulletInfo);
	this->addChild(pBullet);
	m_OtherBullets.PushBack(pBullet);
}

void BattleFieldLayer::CmdBattleFieldDeathAck(ICommand* cmd) {
	const BattleFieldDeathAck* pBattleFieldDeathAck = cmd->CastCommand<BattleFieldDeathAck*>();

	// �ڱ� �ڽ��� ���
	if (_Client->GetCharacterUID() == pBattleFieldDeathAck->CharacterUID) {
		m_fRevivalLeftTime = pBattleFieldDeathAck->RevivalLeftTime / 1000.0f;
		m_pChatBox->AddNoticeMessage("��� - ��� �� ��Ȱ�մϴ�.");

		m_pTank->setMoveable(false);
		m_pTank->setFireable(false);
		m_pTank->setVisible(false);

		// �Ƕ��� ���� ������ ��� Ȱ�� ����
		m_pClickPrevenButton->setEnabled(true);
		m_pClickPrevenButton->setVisible(true);
	} else {
		// �ٸ� ����� ���
		for (int i = 0; i < m_OtherPlayers.Size(); i++) {
			if (m_OtherPlayers[i]->GetCharacterUID() == pBattleFieldDeathAck->CharacterUID) {
				m_OtherPlayers[i]->setVisible(false);
				return;
			}
		}
	}
}

void BattleFieldLayer::CmdBattleFieldRevivalSyn(ICommand* cmd) {
	BattleFieldRevivalSyn* pBattleFieldRevivalSyn = cmd->CastCommand<BattleFieldRevivalSyn*>();
	// �ڱ� �ڽ��� ���
	if (_Client->GetCharacterUID() == pBattleFieldRevivalSyn->CharacterUID) {

		// �Ƕ��� �����ؼ� Ȱ�������ϵ��� ��
		m_pClickPrevenButton->setEnabled(false);			// �̷��� �θ� Ŭ�� ����
		m_pClickPrevenButton->setVisible(false);			// ó�� �Ⱥ��̵���

		m_pTank->setVisible(true);
		m_pTank->setMoveable(true);
		m_pTank->UpdateTankMove(pBattleFieldRevivalSyn->RevivalMove);
		m_pTank->SetCharacterUID(_Client->GetCharacterUID());


		if (m_eRoomState == RoomState::Playing) {
			m_pTank->setFireable(true);		// ���� ���� ��쿡�� �Ѿ� ��� ���
		} 
		
	} else {
		// �ٸ� ����� ���
		for (int i = 0; i < m_OtherPlayers.Size(); i++) {
			if (m_OtherPlayers[i]->GetCharacterUID() == pBattleFieldRevivalSyn->CharacterUID) {
				UpdateTankMove(pBattleFieldRevivalSyn->RevivalMove);
				return;
			}
		}
	}
	
}

void BattleFieldLayer::CmdBattleFieldStatisticsUpdateSyn(ICommand* cmd) const {
	const BattleFieldStatisticsUpdateSyn* pBattleFieldStatisticsUpdateSyn = cmd->CastCommand<BattleFieldStatisticsUpdateSyn*>();

	// std::array ���� �����ѵ� �׷��� ���� �����Ѱ� �Ẹ��.
	JCore::Vector<BattleInfo> sortVector(pBattleFieldStatisticsUpdateSyn->Count);

	for (int i = 0; i < pBattleFieldStatisticsUpdateSyn->Count; i++) {
		sortVector.PushBack(pBattleFieldStatisticsUpdateSyn->Info[i]);
	}

	int iIndexer = 0;
	sortVector.Extension().
				Sorted([](BattleInfo& lhs, BattleInfo& rhs) { return lhs.Kill > rhs.Kill; }).
				ForEach([&iIndexer, this](BattleInfo& info) {
					m_pNameText[iIndexer]->setText(GetRoomMemberName(info.CharacterUID).Source());
					m_pNameText[iIndexer]->setVisible(true);
					m_pKillText[iIndexer]->setText(StringUtils::format("%dų", info.Kill));
					m_pKillText[iIndexer]->setVisible(true);
					m_pDeathText[iIndexer]->setText(StringUtils::format("%d����", info.Death));
					m_pDeathText[iIndexer]->setVisible(true);
					iIndexer++;
				});

	// ������ �� ���� ��� ������ ��������� �Ⱥ��̵��� �Ѵ�.
	for (int i = iIndexer; i < ROOM_MAX_PLAYER_COUNT; i++) {
		m_pNameText[i]->setVisible(false);
		m_pKillText[i]->setVisible(false);
		m_pDeathText[i]->setVisible(false);
	}
}
