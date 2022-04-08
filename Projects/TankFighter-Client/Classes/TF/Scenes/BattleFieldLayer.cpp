/*
 * �ۼ��� : ������
 */


#include <TF/Scenes/BattleFieldLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>
#include <JCore/Ascii.h>

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
	this->addChild(m_pTank);

	m_pTimeText = Text::create(ms_pPlayWaitLeftTimeFormat, FONT_PATH_DEFAULT, 25);
	m_pTimeText->setColor(ColorList::Airsuperiorityblue_v);
	m_pTimeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pTimeText->setVisible(true);
	m_pTimeText->setPosition({ 500, 400 });
	this->addChild(m_pTimeText);

	// �ڱ� �ڽ��� ������ �ִ� ����ŭ �̸� ��ũ�� �����س��´�.
	for (int i = 0; i < ROOM_MAX_PLAYER_COUNT - 1; i++) {
		Tank* pTank = Tank::create(INVALID_UID, this);
		pTank->setVisible(false);
		this->addChild(pTank);
		m_OtherPlayers.PushBack(pTank);
	}

	m_pLeaveChannelButton = TextButton::create(200, 50, "���� ������", 16);
	m_pLeaveChannelButton->setBackgroundColor(ColorList::Bluegray_v);
	m_pLeaveChannelButton->setFontColor(ColorList::Darkorange_v);
	m_pLeaveChannelButton->setAnchorPoint(Vec2::ZERO);
	m_pLeaveChannelButton->setPosition({ 800, 0 });
	m_pLeaveChannelButton->setClickEvent(CC_CALLBACK_1(BattleFieldLayer::OnClickedLeaveGameButton, this));
	this->addChild(m_pLeaveChannelButton);
	this->scheduleUpdate();
	return true;
}

void BattleFieldLayer::update(float delta) {
	// ====================================================== ��ũ�� ��ġ ������Ʈ
	m_fSendTankMoveSynTime += delta;

	m_pTank->updatePosition(delta);
	m_pTank->updateRotation(delta);

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

	// ���� �ֱ⸶�� ������ ��ũ ������ ����������.
	if (m_fSendTankMoveSynTime >= ms_iPlayerSendMoveStatePacketDelay) {
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
		//m_pTimeText->setVisible(false);
	}
}

void BattleFieldLayer::UpdateTankMove(int characterUID, TankMove& move) {
	if (characterUID == _Client->GetCharacterUID()) {
		return;
	}

	for (int i = 0; i < m_OtherPlayers.Size(); i++) {
		Tank* pOtherTank = m_OtherPlayers[i];

		if (!pOtherTank->HasOwner() || pOtherTank->GetCharacterUID() == characterUID) {
			pOtherTank->SetCharacterUID(characterUID);
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





/* =================================================================================
 *
 *                             ��� ��Ŷ ó��
 *
 * ================================================================================*/



void BattleFieldLayer::CmdBattleFieldLoadAck(ICommand* cmd) {
	BattleFieldLoadAck* pBattleFieldLoadAck = cmd->CastCommand<BattleFieldLoadAck*>();

	m_eRoomState = RoomState::PlayWait;
	m_fLeftTime = pBattleFieldLoadAck->LeftTime;

	m_pTank->setVisible(true);
	m_pTank->UpdateTankMove(pBattleFieldLoadAck->InitialMove);
	m_pTank->setMoveable(true);
}

void BattleFieldLayer::CmdBattleFieldLeaveAck(ICommand* cmd) {
	BattleFieldLeaveAck* pBattleFieldLeaveAck = cmd->CastCommand<BattleFieldLeaveAck*>();

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
		UpdateTankMove(pBattileFieldTankUpdateSyn->CharacterUID[i], pBattileFieldTankUpdateSyn->Move[i]);
	}

	// ������Ʈ �ȵ� �ֵ��� ���� �ֵ��̹Ƿ� �Ⱥ��̵��� ��
	for (int i = 0; i < m_OtherPlayers.Size(); i++) {
		if (!m_OtherPlayers[i]->IsUpdated()) {
			m_OtherPlayers[i]->setVisible(false);
		}
	}
}
