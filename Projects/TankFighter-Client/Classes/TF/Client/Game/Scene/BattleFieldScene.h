/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:30:51 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Scene/BaseScene.h>
#include <TF/Client/Game/Contents/Room.h>
#include <TF/Client/Game/Contents/Tank.h>
#include <TF/Client/Game/Contents/Bullet.h>

#include "TF/Common/Command.h"

class BattleFieldScene : public BaseScene
{
public:
	BattleFieldScene(Room* room);
	~BattleFieldScene() override;

	static BattleFieldScene* create(Room* room);

	bool init() override;
	
	void update(float delta) override;
	void updateEntities(float delta);
	void updateEntitiesCollision(float delta);
	void updateTimerText();
	void updateRevivalTime(float delta);

	void onEnterTransitionDidFinish() override;
	void onTankFire(Tank* tank);
	void onClickedChatSendButton(ChatBox* chatBox);
	void onClickedLeaveGameButton(TextButton* btn);
	void onRoomStateChanged(RoomState state);

	Type getType() const override { return Type::BattleField; }

	Tank* getTankByCharacterPrimaryKey(int characterPrimaryKey);			// 해당 캐릭터 프라이머리 키의 탱크 반환
	Tank* getAvilableOtherTank();												// 사용가능한 빈 탱크 반환

	float getRemainTimeOfNextState();
	float getRemainTimeOfRevival();

	void removeTank(int characterPrimaryKey);
	void spawnTank(const TankMoveNet& move);
	void spawnBullet(const BulletInfoNet& info);

	void syncTime(const JCore::TimeSpan& gameTime);
	void syncMove(TankMoveNet* moves, int count);

	void refreshUIByRoomState();
	void refreshStatistics(BattleStatisticsNet* statistics, int count);

	void addChatMssage(const char* str);

	void judge(int winnerCharacterPrimaryKey);
private:
	Room* m_pRoom;
	Tank* m_pMyTank;

	JCore::TimeSpanF m_GameTime;
	JCore::TimeSpanF m_RevivalTime;

	ChatBox* m_pChatBox;
	TextButton* m_pLeaveChannelButton;
	cocos2d::Layer* m_pEntityLayer;

	cocos2d::ui::Text* m_pTimeText;						// 남은시간 보여주는 텍스트
	cocos2d::ui::Text* m_pNoticeText;					// 배틀필드 이벤트 공지 텍스트
	cocos2d::ui::Text* m_pRevivalText;					// 죽고나서 부활할때까지 뜨는 텍스트
	cocos2d::ui::Button* m_pClickPrevenButton;			// 죽고나서 모든 활동을 못하게하기 위한 판때기

	cocos2d::ui::Text* m_pNameText[Const::Room::MaxPlayerCount];
	cocos2d::ui::Text* m_pKillText[Const::Room::MaxPlayerCount];
	cocos2d::ui::Text* m_pDeathText[Const::Room::MaxPlayerCount];

	JCore::Vector<Bullet*>	m_vMyBullets;	// 내가 쏜 총알들
	JCore::Vector<Bullet*>	m_vOtherBullets;	// 다른 플레이어가 쏜 총알들
	JCore::Vector<Tank*>	m_vOtherPlayers;	// 다른 플레이어 탱크
	
};
