/*
 * 작성자 : 윤정도
 */


#include <TF/Scenes/BattleFieldLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>
#include <JCore/Ascii.h>

#include "TF/Object/Bullet.h"

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
	
	// 자기 자신을 제외한 최대 수만큼 미리 탱크를 생성해놓는다.
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
	m_pClickPrevenButton->setEnabled(false);			// 이렇게 두면 클릭 가능
	m_pClickPrevenButton->setVisible(false);			// 처음 안보이도록
	m_pClickPrevenButton->setContentSize({ MAP_WIDTH, MAP_HEIGHT });
	m_pClickPrevenButton->setSize({ MAP_WIDTH, MAP_HEIGHT });
	m_pClickPrevenButton->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pClickPrevenButton, 100);



	m_pLeaveChannelButton = TextButton::create(200, 50, "게임 나가기", 16);
	m_pLeaveChannelButton->setBackgroundColor(ColorList::Bluegray_v);
	m_pLeaveChannelButton->setFontColor(ColorList::Black_v);
	m_pLeaveChannelButton->setAnchorPoint(Vec2::ZERO);
	m_pLeaveChannelButton->setPosition({ 800, 0 });
	m_pLeaveChannelButton->setClickEvent(CC_CALLBACK_1(BattleFieldLayer::OnClickedLeaveGameButton, this));
	this->addChild(m_pLeaveChannelButton);

	for (int i = 0; i < 4; i++) {
		m_pNameText[i] = Text::create("말동무 동무", FONT_PATH_DEFAULT, 14);
		m_pNameText[i]->setColor(ColorList::Blizzardblue_v);
		m_pNameText[i]->setAnchorPoint(Vec2::ZERO);
		m_pNameText[i]->setContentSize({ 100.0f, 0.0 });
		m_pNameText[i]->setVisible(false);
		m_pNameText[i]->setPosition({ 0, 400.0f - i * 20.0f });
		this->addChild(m_pNameText[i], 100);

		m_pKillText[i] = Text::create("0킬", FONT_PATH_DEFAULT, 14);
		m_pKillText[i]->setColor(ColorList::Amber_v);
		m_pKillText[i]->setAnchorPoint(Vec2::ZERO);
		m_pKillText[i]->setVisible(false);
		m_pKillText[i]->setPosition({ m_pNameText[i]->getPosition().x + m_pNameText[i]->getContentSize().width + 20.0f, 400.0f - i * 20.0f });
		this->addChild(m_pKillText[i], 100);

		m_pDeathText[i] = Text::create("0데스", FONT_PATH_DEFAULT, 14);
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
	// ====================================================== 탱크들 위치 업데이트
	m_fSendTankMoveSynTime += delta;

	m_pTank->updatePosition(delta);
	m_pTank->updateRotation(delta);
	m_pTank->updateShotDelay(delta);

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

	// 일정 주기마다 서버로 탱크 정보를 전송해주자. 안죽어있는 경우
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
		m_pTimeText->setVisible(false);
	}


	// ====================================================== 부활시간 업데이트
	m_fRevivalLeftTime -= delta;

	if (m_fRevivalLeftTime >= 0.0f) {
		m_pRevivalText->setVisible(true);
		m_pRevivalText->setText(StringUtils::format("%.1f초뒤 부활합니다.", m_fRevivalLeftTime));
	} else {
		m_pRevivalText->setVisible(false);
	}


	// ====================================================== 총알 충돌 체크


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

		// 총알이 맵 범위를 벗어난 경우 삭제시킨다.
		if (pBullet->getPosition().x < 0 || pBullet->getPosition().x > MAP_WIDTH ||
			pBullet->getPosition().y < 0 || pBullet->getPosition().y > MAP_HEIGHT) {

			m_OtherBullets.Remove(pBullet);
			this->removeChild(pBullet);
			i--;
			continue;
		}

		if (m_pTank->IsCollide(pBullet) && !IsDeath(m_pTank->GetCharacterUID())) {

			// 내 탱크를 이제 컨트롤 못하게 한다.
			m_pTank->setVisible(false);
			m_pTank->setMoveable(false);
			m_pTank->setFireable(false);
			SetDeath(m_pTank->GetCharacterUID(), true);
			
			SendFn::SendBattleFieldDeathSyn(pBullet->GetCharacterUID());


			// 총알을 제거한다.
			m_OtherBullets.Remove(pBullet);
			this->removeChild(pBullet);
			i--;
			continue;
		}

		for (int j = 0; j < m_OtherPlayers.Size(); j++) {
			Tank* pOther = m_OtherPlayers[j];

			// 다른 탱크와 부딫힌 경우에는 총알만 없앤다.
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

		// 죽은녀석은 건너뜀
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
	PopUp::createInParent("정말로 탈주하시겠습니까?", this, false,
		[]() {
			// 수락시 로비로 이동시켜주자.
			SendFn::SendBattleFieldLeaveSyn();
			_Client->SetRoomUID(INVALID_UID);
			_Client->ChangeScene(SceneType::Lobby);
		},	 
		[]() { }); // 거절 
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
 *                             통신 패킷 처리
 *
 * ================================================================================*/

// 방 멤버 변경시 갑자기 누가 나가거나 할 경우 업데이트 해줘야함
void BattleFieldLayer::CmdUpdateRoomInfoAck(ICommand* cmd) {
	UpdateRoomInfoAck* pUpdateRoomInfoAck = cmd->CastCommand<UpdateRoomInfoAck*>();

	if (pUpdateRoomInfoAck->Count > ROOM_MAX_PLAYER_COUNT) {
		DebugAssertMsg(false, "ROOM_MAX_PLAYER_COUNT 수치 초과하는 Count를 받음");
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

	// 캐릭터 정보 업데이트
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

	// 난입하여 들어오는 경우 방상태에 따라 m_pTimeText 폰트크기와 위치가 다름
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
		// 죽은 사람은 업데이트 안함
		UpdateTankMove(pBattileFieldTankUpdateSyn->Move[i]);
	}

	// 업데이트 안된 애들은 없는 애들이므로 안보이도록 함
	for (int i = 0; i < m_OtherPlayers.Size(); i++) {
		if (!m_OtherPlayers[i]->HasOwner()) {
			m_OtherPlayers[i]->setVisible(false);
			m_OtherPlayers[i]->setPosition(-500, -500);
		}
	}
}

// 썡뚱맞게 방 기능이 있지만 게임 플레이중에 다른 유저가 게임 난입을 할 경우에 이 커맨드를 수행한다.
void BattleFieldLayer::CmdRoomGameStartAck(ICommand* cmd) const {
	RoomGameStartAck* pRoomGameStartAck = cmd->CastCommand<RoomGameStartAck*>();
	FadeOut* pFadeOutAnimation = FadeOut::create(5.0f);
	m_pNoticeText->setOpacity(255);
	m_pNoticeText->runAction(pFadeOutAnimation);
	

	const JCore::String message = StringUtil::Format("%s 님이 ☆ 난입 ☆ 하셨습니다.", pRoomGameStartAck->IntruderInfo.Name);
	m_pNoticeText->setText(message.Source());
	m_pChatBox->AddNoticeMessage(message);
}


void BattleFieldLayer::CmdBattleFieldPlayWaitEndSyn(ICommand* cmd) {
	const BattleFieldPlayWaitEndSyn* pBattleFieldPlayWaitEndSyn = cmd->CastCommand<BattleFieldPlayWaitEndSyn*>();
	FadeOut* pFadeOutAnimation = FadeOut::create(5.0f);
	m_pNoticeText->setOpacity(255);
	m_pNoticeText->runAction(pFadeOutAnimation);

	m_pNoticeText->setText("게임 시작");
	m_pChatBox->AddNoticeMessage("게임 시작");
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
	m_pNoticeText->setText("게임 종료 - 잠시 뒤 로비로 이동합니다.");
	m_pChatBox->AddNoticeMessage("게임 종료 - 잠시 뒤 로비로 이동합니다.");
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

	// 내가 쏜 총알은 업데이트 안함
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

	// 자기 자신인 경우
	if (_Client->GetCharacterUID() == pBattleFieldDeathAck->CharacterUID) {
		m_fRevivalLeftTime = pBattleFieldDeathAck->RevivalLeftTime / 1000.0f;
		m_pChatBox->AddNoticeMessage("사망 - 잠시 뒤 부활합니다.");

		m_pTank->setMoveable(false);
		m_pTank->setFireable(false);
		m_pTank->setVisible(false);

		// 판때기 위에 쒸워서 모든 활동 막음
		m_pClickPrevenButton->setEnabled(true);
		m_pClickPrevenButton->setVisible(true);
	} else {
		// 다른 사람이 경우
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
	// 자기 자신인 경우
	if (_Client->GetCharacterUID() == pBattleFieldRevivalSyn->CharacterUID) {

		// 판때기 제거해서 활동가능하도록 함
		m_pClickPrevenButton->setEnabled(false);			// 이렇게 두면 클릭 가능
		m_pClickPrevenButton->setVisible(false);			// 처음 안보이도록

		m_pTank->setVisible(true);
		m_pTank->setMoveable(true);
		m_pTank->UpdateTankMove(pBattleFieldRevivalSyn->RevivalMove);
		m_pTank->SetCharacterUID(_Client->GetCharacterUID());


		if (m_eRoomState == RoomState::Playing) {
			m_pTank->setFireable(true);		// 게임 중인 경우에만 총알 쏘기 허용
		} 
		
	} else {
		// 다른 사람이 경우
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

	// std::array 쓰면 좋긴한데 그래도 내가 구현한거 써보자.
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
					m_pKillText[iIndexer]->setText(StringUtils::format("%d킬", info.Kill));
					m_pKillText[iIndexer]->setVisible(true);
					m_pDeathText[iIndexer]->setText(StringUtils::format("%d데스", info.Death));
					m_pDeathText[iIndexer]->setVisible(true);
					iIndexer++;
				});

	// 유저가 꽉 안찬 경우 나머지 통계정보는 안보이도록 한다.
	for (int i = iIndexer; i < ROOM_MAX_PLAYER_COUNT; i++) {
		m_pNameText[i]->setVisible(false);
		m_pKillText[i]->setVisible(false);
		m_pDeathText[i]->setVisible(false);
	}
}
