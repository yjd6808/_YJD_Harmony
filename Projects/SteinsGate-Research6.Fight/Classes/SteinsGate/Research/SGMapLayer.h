/*
 * 작성자 : 윤정도
 */
#pragma once

#include <SteinsGate/Research/Tutturu.h>

#include <SteinsGate/Common/Core/Npk/NpkPackage.h>
#include <SteinsGate/Research/SGStruct.h>
#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGProjectile.h>
#include <SteinsGate/Research/SGMonster.h>
#include <SteinsGate/Research/SGObstacle.h>


class SGMapLayer : public SGLayer
{
public:
	SGMapLayer();
	~SGMapLayer() override;

	void loadMap(int mapCode);
	bool init() override;
	bool isCollideWithObstacles(const SGRect& rect);

	SGCharacter* findNearestCharacterInRadious(SGActor* stdActor, float radious, Out_ float& enemyDist);

	// 1개이상 있으면 true 리턴
	bool collectEnemiesInInstantAttackBox(SGActor* attacker, const SGActorRect& rect, Out_ SGVector<SGHitInfo>& hitMonsters);

	static SGMapLayer* create(int mapCode);

	void update(float dt) override;
	void updateZOrder(float dt);
	void updateActors(float dt);
	void updatePlayerProjectiles(float dt);

	void runFrameEvent(SGActor* runner, FrameEventType_t frameEventType, int frameEventId);
	SGCharacter* createCharacter(CharacterType_t characterType, float x, float y, SGCharacterInfo& info);
	SGProjectile* createProejctile(SGActor* spawner, int projectileId);
	void createAttackBox(SGActor* spawner, int hitBoxId);
	SGMonster* createMonster(int code, int aiCode, float x, float y);
	SGObstacle* createObstacle(int code, float x, float y);

	
	void registerZOrderActor(SGActor* actor);
	void registerCharacter(SGCharacter* character);
	void registerPlayerProjectile(SGProjectile* projectile);
	void registerMonster(SGMonster* mosnter);
	void registerObstacle(SGObstacle* obstacle);
	void registerPhysicsActor(SGPhysicsActor* physicsActor);
	void registerCleanUp(SGActor* actor);
	

	void unregisterZOrderActor(SGActor* actor);
	void unregisterPlayerProjectile(SGProjectile* projectile);
	void unregisterMonster(SGMonster* mosnter);
	void unregisterObstacle(SGObstacle* obstacle);
	void unregisterPhysicsActor(SGPhysicsActor* physicsActor);

	void cleanUpActors();
	void cleanUpProjectile(SGProjectile* projectile);
	
	void cleanUpMonster(SGMonster* monster);
	void cleanUpObstacle(SGObstacle* obstacle);
	void cleanUpCharacter(SGCharacter* character);

	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;

	SGMapInfo* getMapInfo();

public:
	// 맵관련 정보
	SGMapInfo* m_pMapInfo;
	SGVector<SGFrameTexture*> m_vTileTextures;
	
	// TODO: 액터 박스를 구현해서 따로 관리해줄 필요가 있음
	SGPlayer* m_pPlayer;
	SGVector<SGActor*> m_vZOrderedActors;			// Z오더를 고유 ID처럼 사용해도 될듯? 이진탐색으로 삭제 함 댈것같다.
	SGVector<SGProjectile*> m_vPlayerProjectiles;	// 플레이어가 만든 충돌체
	SGVector<SGProjectile*> m_vEnmemyProjectiles;	// 적이 만든 충돌체
	SGVector<SGMonster*> m_vMonsters;
	SGVector<SGObstacle*> m_vCollidableObstacles;
	SGVector<SGCharacter*> m_vCharacters;
	SGVector<SGPhysicsActor*> m_vPhysicsActors;

	SGHashMap<SGActor*, SGActor*> m_hRemoveActorMap;	// 디버깅용, 중복 큐잉 방지용
	SGArrayQueue<SGActor*> m_qRemovedActors;

	float m_fZReorderTime;
};



