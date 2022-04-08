/*
 * 작성자 : 윤정도
 */


#include <TF/Scenes/BattleFieldLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>
#include <JCore/Ascii.h>

const char* BattleFieldLayer::ms_pPlayWaitLeftTimeFormat = "다른 플레이어가 모두 입장할때까지 기다려주세요.\n남은시간 : %.1f초";
const char* BattleFieldLayer::ms_pPlayingLeftTimeFormat = "게임 종료까지 남은시간 : %.1f초";
const char* BattleFieldLayer::ms_pEndWaitLeftTimeFormat = "방으로 이동하기까지 남은시간 : % .1f초";

void BattleFieldLayer::onEnterTransitionDidFinish() {
	// 배틀필드에 들어오면 
	if (!SendFn::SendBattleFieldLoadSyn()) {
		PopUp::createInParent("배틀필드 정보 요청에 실패하였습니다.", this, false);
	}
}



bool BattleFieldLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	// 탱크를 아무것도 못하는 미리 만들어놓는다.
	m_pTank = Tank::create(_Client->GetCharacterUID(), this);
	m_pTank->setVisible(false);
	this->addChild(m_pTank);

	m_pTimeText = Text::create(ms_pPlayWaitLeftTimeFormat, FONT_PATH_DEFAULT, 25);
	m_pTimeText->setColor(ColorList::Airsuperiorityblue_v);
	m_pTimeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pTimeText->setVisible(true);
	m_pTimeText->setPosition({ 500, 400 });
	this->addChild(m_pTimeText);

	// 자기 자신을 제외한 최대 수만큼 미리 탱크를 생성해놓는다.
	for (int i = 0; i < ROOM_MAX_PLAYER_COUNT - 1; i++) {
		Tank* pTank = Tank::create(INVALID_UID, this);
		pTank->setVisible(false);
		this->addChild(pTank);
		m_OtherPlayers.PushBack(pTank);
	}

	m_pLeaveChannelButton = TextButton::create(200, 50, "게임 나가기", 16);
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
	// ====================================================== 탱크들 위치 업데이트
	m_fSendTankMoveSynTime += delta;

	m_pTank->updatePosition(delta);
	m_pTank->updateRotation(delta);

	Vec2 myPos = m_pTank->getPosition();

	// 맵 밖으로 나가는거 방지
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

	// 일정 주기마다 서버로 탱크 정보를 전송해주자.
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

	// ====================================================== 남은시간 텍스트 업데이트
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
	PopUp::createInParent("정말로 탈주하시겠습니까?", this, false,
		[]() {
			// 수락시 로비로 이동시켜주자.
			SendFn::SendBattleFieldLeaveSyn();
			_Client->SetRoomUID(INVALID_UID);
			_Client->ChangeScene(SceneType::Lobby);
		},	 
		[]() { }); // 거절 
}





/* =================================================================================
 *
 *                             통신 패킷 처리
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

	// 업데이트 안된 애들은 없는 애들이므로 안보이도록 함
	for (int i = 0; i < m_OtherPlayers.Size(); i++) {
		if (!m_OtherPlayers[i]->IsUpdated()) {
			m_OtherPlayers[i]->setVisible(false);
		}
	}
}
