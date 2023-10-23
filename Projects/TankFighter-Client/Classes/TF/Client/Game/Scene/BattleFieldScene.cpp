/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:32:29 PM
 * =====================
 *
 */

#include "Pch.h"
#include "BattleFieldScene.h"

#include <TF/Common/Structure.h>
#include <TF/Common/Const.h>
#include <TF/Client/Net/Send/S_GAME.h>
#include <TF/Client/Game/Scene/ChannelScene.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_CCUI;

BattleFieldScene::BattleFieldScene(Room* room)
	: m_pRoom(room)
	, m_pMyTank(nullptr)
	, m_pChatBox(nullptr)
	, m_pLeaveChannelButton(nullptr)
	, m_pEntityLayer(nullptr)
	, m_pTimeText(nullptr)
	, m_pNoticeText(nullptr)
	, m_pRevivalText(nullptr)
	, m_pClickPrevenButton(nullptr)
	, m_pNameText{}
	, m_pKillText{}
	, m_pDeathText{}
{}

BattleFieldScene::~BattleFieldScene()
{}

BattleFieldScene* BattleFieldScene::create(Room* room) {
	if (room == nullptr)
		return nullptr;

	BattleFieldScene* pScene = dbg_new BattleFieldScene(room);

	if (pScene->init()) {
		pScene->autorelease();
		return pScene;
	}

	delete pScene;
	return nullptr;
}

void BattleFieldScene::onEnterTransitionDidFinish() {
	BaseScene::onEnterTransitionDidFinish();

	if (S_GAME::SEND_CS_LoadBattleFieldInfo(Core::GameClient->GetRoomAccessId()) == false) {
		PopUp::createInParent("배틀필드 정보 요청이 실패하였습니다.", this, false);
	}
}

bool BattleFieldScene::init() {

	if (!BaseScene::init()) {
		return false;
	}

	m_pEntityLayer = Layer::create();
	this->addChild(m_pEntityLayer);

	// 탱크를 아무것도 못하는 미리 만들어놓는다.
	m_pMyTank = Tank::create();
	m_pMyTank->free();
	m_pMyTank->setFireCallback(CC_CALLBACK_1(BattleFieldScene::onTankFire, this));
	m_pEntityLayer->addChild(m_pMyTank, 2);

	// 자기 자신을 제외한 최대 수만큼 미리 탱크를 생성해놓는다.
	for (int i = 0; i < Const::Room::MaxPlayerCount - 1; i++) {
		Tank* pTank = Tank::create();
		pTank->setVisible(false);
		m_pEntityLayer->addChild(pTank, 2);
		m_vOtherTanks.PushBack(pTank);
	}

	m_pTimeText = Text::create("", Const::Resource::FontName, 25);
	m_pTimeText->setColor(ColorList::Airsuperiorityblue_v);
	m_pTimeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pTimeText->setVisible(false);
	m_pTimeText->setPosition({ 500, 400 });
	m_pUILayer->addChild(m_pTimeText);

	m_pRevivalText = Text::create("", Const::Resource::FontName, 20);
	m_pRevivalText->setColor(ColorList::Africanviolet_v);
	m_pRevivalText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pRevivalText->setVisible(false);
	m_pRevivalText->setPosition({ 500.0f, 250.0f });
	m_pUILayer->addChild(m_pRevivalText);

	m_pNoticeText = Text::create("", Const::Resource::FontName, 25);
	m_pNoticeText->setColor(ColorList::Arylideyellow_v);
	m_pNoticeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pNoticeText->setOpacity(0);
	m_pNoticeText->setPosition({ 500, 450 });
	m_pUILayer->addChild(m_pNoticeText);

	m_pChatBox = ChatBox::create(ColorList::BlackShadows_v, { 400, 200 });
	m_pChatBox->setAnchorPoint(Vec2::ZERO);
	m_pChatBox->SetSendButtonClickEvent(CC_CALLBACK_1(BattleFieldScene::onClickedChatSendButton, this));
	m_pChatBox->SetContentSize({ 400, 200 });
	m_pUILayer->addChild(m_pChatBox);

	m_pClickPrevenButton = Button::create(Const::Resource::WhiteRectFileName, Const::Resource::WhiteRectFileName, Const::Resource::WhiteRectFileName);
	m_pClickPrevenButton->setScale9Enabled(true);
	m_pClickPrevenButton->setColor(Color3B::GRAY);
	m_pClickPrevenButton->setOpacity(60);
	m_pClickPrevenButton->setEnabled(false);			// 이렇게 두면 클릭 가능
	m_pClickPrevenButton->setVisible(false);			// 처음 안보이도록
	m_pClickPrevenButton->setContentSize({ Const::Map::Width, Const::Map::Height });
	m_pClickPrevenButton->setAnchorPoint(Vec2::ZERO);
	m_pUILayer->addChild(m_pClickPrevenButton, 100);

	m_pLeaveChannelButton = TextButton::create(200, 50, "게임 나가기", 16);
	m_pLeaveChannelButton->setBackgroundColor(ColorList::Bluegray_v);
	m_pLeaveChannelButton->setFontColor(ColorList::Black_v);
	m_pLeaveChannelButton->setAnchorPoint(Vec2::ZERO);
	m_pLeaveChannelButton->setPosition({ 800, 0 });
	m_pLeaveChannelButton->setClickEvent(CC_CALLBACK_1(BattleFieldScene::onClickedLeaveGameButton, this));
	m_pUILayer->addChild(m_pLeaveChannelButton);

	for (int i = 0; i < Const::Room::MaxPlayerCount; i++) {
		m_pNameText[i] = Text::create("말동무 동무", Const::Resource::FontName, 14);
		m_pNameText[i]->setColor(ColorList::Blizzardblue_v);
		m_pNameText[i]->setAnchorPoint(Vec2::ZERO);
		m_pNameText[i]->setContentSize({ 100.0f, 0.0 });
		m_pNameText[i]->setVisible(false);
		m_pNameText[i]->setPosition({ 0, 400.0f - i * 20.0f });
		m_pUILayer->addChild(m_pNameText[i], 100);

		m_pKillText[i] = Text::create("0킬", Const::Resource::FontName, 14);
		m_pKillText[i]->setColor(ColorList::Amber_v);
		m_pKillText[i]->setAnchorPoint(Vec2::ZERO);
		m_pKillText[i]->setVisible(false);
		m_pKillText[i]->setPosition({ m_pNameText[i]->getPosition().x + m_pNameText[i]->getContentSize().width + 20.0f, 400.0f - i * 20.0f });
		m_pUILayer->addChild(m_pKillText[i], 100);

		m_pDeathText[i] = Text::create("0데스", Const::Resource::FontName, 14);
		m_pDeathText[i]->setColor(ColorList::Alabaster_v);
		m_pDeathText[i]->setAnchorPoint(Vec2::ZERO);
		m_pDeathText[i]->setVisible(false);
		m_pDeathText[i]->setPosition({ m_pKillText[i]->getPosition().x + m_pKillText[i]->getContentSize().width + 20.0f, 400.0f - i * 20.0f});
		m_pUILayer->addChild(m_pDeathText[i], 100);
	}

	return true;
}

void BattleFieldScene::update(float delta) {
	BaseScene::update(delta);

	updateEntities(delta);
	updateEntitiesCollision(delta);
	updateTimerText();
	updateRevivalTime(delta);

	m_GameTime.Second += delta;
}

void BattleFieldScene::updateEntities(float delta) {
	m_pMyTank->update(delta);

	for (int i = 0; i < m_vMyBullets.Size(); ++i) {
		m_vMyBullets[i]->update(delta);
	}

	for (int i = 0; i < m_vOtherBullets.Size(); ++i) {
		m_vOtherBullets[i]->update(delta);
	}

	for (int i = 0; i < m_vOtherTanks.Size(); ++i) {
		m_vOtherTanks[i]->update(delta);
	}
}

void BattleFieldScene::updateEntitiesCollision(float delta) {

	// 게임중일때만 충돌처리
	if (m_pRoom->getRoomState() != RoomState::Playing)
		return;

	for (int i = 0; i < m_vMyBullets.Size(); ) {
		Bullet* pMyBullet = m_vMyBullets[i];
		// 맵 바깥으로 벗어난 총알 제거
		if (pMyBullet->isOutOfMap()) {
			m_vMyBullets.Remove(pMyBullet);
			m_pEntityLayer->removeChild(pMyBullet);
		} else {
			i++;
		}
	}

	for (int i = 0; i < m_vOtherBullets.Size(); i++) {
		Bullet* pBullet = m_vOtherBullets[i];

		// 맵 바깥으로 벗어난 총알 제거
		if (pBullet->isOutOfMap()) {
			m_vOtherBullets.Remove(pBullet);
			m_pEntityLayer->removeChild(pBullet);
			i--;
			continue;
		}

		if (m_pMyTank->isCollide(pBullet) && !m_pMyTank->isDeath()) {
			// 내 탱크를 이제 컨트롤 못하게 한다.

			S_GAME::SEND_CS_BattleFieldDeath(pBullet->getCharacterPrimaryKey());

			// 총알을 제거한다.
			m_vOtherBullets.Remove(pBullet);
			m_pEntityLayer->removeChild(pBullet);
			i--;
			continue;
		}

		for (int j = 0; j < m_vOtherTanks.Size(); j++) {
			Tank* pOtherTank = m_vOtherTanks[j];

			// 다른 탱크와 부딫힌 경우에는 총알만 없앤다.
			if (pOtherTank->isCollide(pBullet) && !pOtherTank->isDeath()) {
				m_vOtherBullets.Remove(pBullet);
				m_pEntityLayer->removeChild(pBullet);
				i--;
				break;
			}
		}
	}
}

void BattleFieldScene::updateTimerText() {

	const float fLeftTime = getRemainTimeOfNextState();
	const float fLeftRevivalTime = getRemainTimeOfRevival();

	if (fLeftTime > 0) {
		m_pTimeText->setVisible(true);
		switch (m_pRoom->getRoomState()) {
		case RoomState::PlayWait: m_pTimeText->setString(StringUtils::format(Const::String::PlayWaitLeftTimeFormat, fLeftTime));	break;
		case RoomState::Playing: m_pTimeText->setString(StringUtils::format(Const::String::PlayingLeftTimeFormat, fLeftTime));	break;
		case RoomState::EndWait: m_pTimeText->setString(StringUtils::format(Const::String::EndWaitLeftTimeFormat, fLeftTime));	break;
		default: ;
		}
	} else {
		m_pTimeText->setVisible(false);
	}

	if (fLeftRevivalTime > 0.0f) {
		m_pClickPrevenButton->setVisible(true);
		m_pClickPrevenButton->setEnabled(true);

		m_pRevivalText->setVisible(true);
		m_pRevivalText->setString(StringUtils::format(Const::String::RevivalLeftTimeFormat, fLeftRevivalTime));
	} else {
		m_pClickPrevenButton->setVisible(false);
		m_pClickPrevenButton->setEnabled(false);

		m_pRevivalText->setVisible(false);
	}
}

void BattleFieldScene::updateRevivalTime(float delta) {
	if (!m_pMyTank->isDeath())
		return;

	m_RevivalTime.Second += delta;
}

void BattleFieldScene::onTankFire(Tank* tank) {
	static constexpr int RandomColorCount = 12;
	static Color3B colorList[RandomColorCount]{
		ColorList::Alloyorange_v,
		ColorList::Androidgreen_v,
		ColorList::Amaranthpink_v,
		ColorList::Bitterlemon_v,
		ColorList::BlueNCS_v,
		ColorList::Bottlegreen_v,
		ColorList::Brightlilac_v,
		ColorList::Canary_v,
		ColorList::Celeste_v,
		ColorList::Chartreuseweb_v,
		ColorList::Coolgrey_v,
		ColorList::Cybergrape_v
	};


	BulletInfoNet info;
	Vec2 firePos = tank->getFirePosition();

	info.X = firePos.x;
	info.Y = firePos.y;
	info.Rotation = tank->getRotation();
	info.Diameter = Random::GenerateF(5.0f, 20.0f);
	info.MoveSpeed = Random::GenerateF(70.0f, 300.0f);
	info.CharacterPrimaryKey = tank->getCharacterPrimaryKey();
	info.Color = ColorUtil::ToIn32U(colorList[Random::GenerateInt(0, RandomColorCount)]);
	S_GAME::SEND_CS_BattleFieldFire(info);
}


void BattleFieldScene::onClickedChatSendButton(ChatBox* chatBox) {
	JCore::String message = chatBox->GetEditBox()->getText();
	if (message.Length() == 0) {
		return;
	}

	S_GAME::SEND_CS_ChatMessage(message.Source());
}

void BattleFieldScene::onClickedLeaveGameButton(TextButton* btn) {
	PopUp::createInParent("정말로 탈주하시겠습니까?", this, false,
		[] { S_GAME::SEND_CS_RoomLeave(); },
		[] { }
	);
}

void BattleFieldScene::onRoomStateChanged(RoomState state) {
	if (state == RoomState::Playing) {
		Text* pAnimatedText = Text::create("박진감 넘치는 전투 시작!!!", Const::Resource::FontName, 48);
		pAnimatedText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		pAnimatedText->setPositionX(Const::Window::Width / 2.0f);
		pAnimatedText->setPositionY(Const::Window::Height - 150.0f);
		pAnimatedText->runAction(FadeOut::create(2.0f));
		m_pUILayer->addChild(pAnimatedText);
	}
}

Tank* BattleFieldScene::getTankByCharacterPrimaryKey(int characterPrimaryKey) {
	if (m_pMyTank->getCharacterPrimaryKey() == characterPrimaryKey) {
		return m_pMyTank;
	}

	for (int i = 0; i < m_vOtherTanks.Size(); ++i) {
		if (m_vOtherTanks[i]->getCharacterPrimaryKey() == characterPrimaryKey) {
			return m_vOtherTanks[i];
		}
	}
	return nullptr;
}

Tank* BattleFieldScene::getAvilableOtherTank() {
	for (int i = 0; i < m_vOtherTanks.Size(); ++i) {
		if (m_vOtherTanks[i]->getCharacterPrimaryKey() == Const::InvalidValue) {
			return m_vOtherTanks[i];
		}
	}

	return nullptr;
}

float BattleFieldScene::getRemainTimeOfNextState() {
	float fGameTime = float(m_GameTime.GetTotalSeconds());

	switch (m_pRoom->getRoomState()) {
	case RoomState::PlayWait: return Const::BattleField::PlayWaitTime - fGameTime;
	case RoomState::Playing: return Const::BattleField::PlayingTime - fGameTime;
	case RoomState::EndWait: return Const::BattleField::EndWaitTime- fGameTime;
	default: ;
	}
	return -1;
}

float BattleFieldScene::getRemainTimeOfRevival() {
	if (!m_pMyTank->isDeath())
		return -1;

	float FRevivalTime = float(m_RevivalTime.GetTotalSeconds());
	return Const::BattleField::RevivalTime - FRevivalTime;
}

void BattleFieldScene::removeTank(int characterPrimaryKey) {
	Tank* pTank = getTankByCharacterPrimaryKey(characterPrimaryKey);
	if (pTank == nullptr) {
		_LogWarn_("방에 없어서 제거 불가능");
		return;
	}
	pTank->free();
}

void BattleFieldScene::spawnTanks(TankMoveNet* moves, int count) {

	for (int i = 0; i < count; ++i) {
		const TankMoveNet& move = moves[i];
		RoomCharacterInfo* pInfo = m_pRoom->getRoomMemberByPrimaryKey(move.CharacterPrimaryKey);

		if (pInfo == nullptr) {
			_LogWarn_("방에 없어서 소환불가능");
			continue;
		}

		if (pInfo->IsDeath)
			continue;

		if (pInfo->PrimaryKey == Core::GameClient->GetCharacterPrimaryKey()) {

			// 중복 spawn 방지
			if (pInfo->PrimaryKey == m_pMyTank->getCharacterPrimaryKey()) {
				continue;
			}
			m_RevivalTime.Second = 0;
			m_pMyTank->spawn(move.CharacterPrimaryKey, move);
			continue;
		}

		// 중복 spawn 방지
		if (getTankByCharacterPrimaryKey(move.CharacterPrimaryKey) != nullptr) {
			continue;
		}

		Tank* pOtherTank = getAvilableOtherTank();
		if (pOtherTank == nullptr) {
			_LogWarn_("소환가능한 탱크가 없습니다.");
			continue;
		}

		pOtherTank->spawn(move.CharacterPrimaryKey, move);
	}
}

void BattleFieldScene::spawnBullet(const BulletInfoNet& info) {

	Tank* pTank = getTankByCharacterPrimaryKey(info.CharacterPrimaryKey);
	if (pTank == nullptr) {
		_LogWarn_("총알을 쏠 탱크를 찾을 수 없습니다.");
		return;
	}

	Bullet* pBullet = Bullet::create();
	pBullet->setNetInfo(info);
	m_pEntityLayer->addChild(pBullet);
	if (pTank == m_pMyTank)
		m_vMyBullets.PushBack(pBullet);
	else
		m_vOtherBullets.PushBack(pBullet);
}

void BattleFieldScene::syncTime(const TimeSpan& gameTime) {
	m_GameTime.Second = float(gameTime.GetTotalSeconds());
	updateTimerText();
}

void BattleFieldScene::syncMove(const TankMoveNet& move) {
	Tank* pTank = getTankByCharacterPrimaryKey(move.CharacterPrimaryKey);
	if (pTank == m_pMyTank) return;	// 내껀 내가 조종하므로 무시
	if (pTank == nullptr) return;	// 찾을 수 없어도 무시
	pTank->setTankMove(move);
}

void BattleFieldScene::refreshUIByRoomState() {
	switch (m_pRoom->getRoomState()) {
	case RoomState::PlayWait: {
		m_pTimeText->setColor(ColorList::Airsuperiorityblue_v);
		m_pTimeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pTimeText->setFontSize(25);
		m_pTimeText->setPosition({ 500, 400 });
		break;
	}
	case RoomState::Playing: {
		m_pTimeText->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		m_pTimeText->setPosition(Vec2(Const::Map::Width, Const::Map::Height));
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
	default: ;
	}
}

void BattleFieldScene::refreshStatistics(BattleStatisticsNet* statistics, int count) {
	// std::array 쓰면 좋긴한데 그래도 내가 구현한거 써보자.
	JCore::Vector<BattleStatisticsNet> sortVector(count);

	for (int i = 0; i < count; i++) {
		sortVector.PushBack(statistics[i]);
	}

	int iIndexer = 0;
	sortVector.Extension().
				Sorted([](BattleStatisticsNet& lhs, BattleStatisticsNet& rhs) {
					if (lhs.Kill != rhs.Kill) {
						return lhs.Kill > rhs.Kill;
					}

					return lhs.LastKillTime < rhs.LastKillTime;
				}).
				ForEach([&iIndexer, this](BattleStatisticsNet& info) {
					m_pNameText[iIndexer]->setString(m_pRoom->getRoomMemberName(info.CharacterPrimaryKey).Source());
					m_pNameText[iIndexer]->setVisible(true);
					m_pKillText[iIndexer]->setString(StringUtils::format("%d킬", info.Kill));
					m_pKillText[iIndexer]->setVisible(true);
					m_pDeathText[iIndexer]->setString(StringUtils::format("%d데스", info.Death));
					m_pDeathText[iIndexer]->setVisible(true);
					iIndexer++;
				});

	// 유저가 꽉 안찬 경우 나머지 통계정보는 안보이도록 한다.
	for (int i = iIndexer; i < Const::Room::MaxPlayerCount; i++) {
		m_pNameText[i]->setVisible(false);
		m_pKillText[i]->setVisible(false);
		m_pDeathText[i]->setVisible(false);
	}
}

void BattleFieldScene::refreshTanks() {
	// 방 정보에 없는 탱크들은 모두 제거처리함.
	for (int i = 0; i < m_vOtherTanks.Size(); ++i) {
		Tank* pOtherTank = m_vOtherTanks[i];
		const int iCharacterPrimaryKey = pOtherTank->getCharacterPrimaryKey();

		if (iCharacterPrimaryKey == Const::InvalidValue) {
			pOtherTank->free();
			continue;
		}

		if (m_pRoom->getRoomMemberByPrimaryKey(iCharacterPrimaryKey) == nullptr) {
			pOtherTank->free();
		} 
	}
}

void BattleFieldScene::addChatMssage(const char* str) {
	m_pChatBox->AddChatMessage(str);
}

void BattleFieldScene::judge(int winnerCharacterPrimaryKey) {
	Text* pAnimatedText;
	if (m_pMyTank->getCharacterPrimaryKey() == winnerCharacterPrimaryKey) {
		pAnimatedText = Text::create(StringUtils::format("당신은 치열한 전투에서 승리하였습니다."), Const::Resource::FontName, 48);
	} else {
		pAnimatedText = Text::create(StringUtils::format("졌어용 ㅠㅠㅠ"), Const::Resource::FontName, 48);
	}
	pAnimatedText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	pAnimatedText->setPositionX(Const::Window::Width / 2.0f);
	pAnimatedText->setPositionY(Const::Window::Height - 150.0f);
	m_pUILayer->addChild(pAnimatedText);
}
