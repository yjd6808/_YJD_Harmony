/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 6:09:56 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>

#include <SteinsGate/Research/SGStruct.h>
#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGProjectile.h>
#include <SteinsGate/Research/SGMonster.h>
#include <SteinsGate/Research/SGObstacle.h>
#include <SteinsGate/Research/SGEffect.h>

using ActorList = SGVector<SGActor*>;
using CharacterList = SGVector<SGCharacter*>;
using ProjectileList = SGVector<SGProjectile*>;
using MonsterList = SGVector<SGMonster*>;
using PhysicsActorList = SGVector<SGPhysicsActor*>;
using ObstacleList = SGVector<SGObstacle*>;
using EffectList = SGVector<SGEffect*>;

class SGMapLayer;
class SGActorBox
{
	SGActorBox();
public:
	~SGActorBox();

	static SGActorBox* getInstance() {
		static SGActorBox* s_pInstance = nullptr;

		if (s_pInstance == nullptr) {
			s_pInstance = new SGActorBox;
		}
		return s_pInstance;
	}

	void init(SGMapLayer* mapLayer);
	
	void update(float dt);
	
	void updateActors(float dt);
	void updateZOrder(float dt);
	void updateCleanUp();

	void clear();


	SGCharacter* createCharacterOnMap(CharType_t charType, float x, float y, SGCharacterInfo& info);
	SGProjectile* createProejctileOnMap(SGActor* spawner, int projectileId);
	SGMonster* createMonsterOnMap(int monsterCode, int aiCode, float x, float y);
	SGObstacle* createObstacleOnMap(int obstacleCode, float x, float y);

	// 위에 다른 애들은 전부 절대 위치 x,y이고
	// 이 함수는 spawner기준 상대좌표로 이펙트를 만든다.
	SGEffect* createEffectOnMapRelative(SGActor* spawner, int effectCode, float offsetX, float offsetY);
	// SGEffect* createEffectOnMapAbsolute(int effectCode, float x, float y);


	void registerCleanUp(SGActor* actor);
	void registerZOrderActor(SGActor* actor);
	void registerProjectile(SGProjectile* projectile);
	void registerCharacter(SGCharacter* character);
	void registerMonster(SGMonster* mosnter);
	void registerObstacle(SGObstacle* obstacle);
	void registerEffect(SGEffect* effect);
	void registerPhysicsActor(SGPhysicsActor* physicsActor);
	void registerActor(SGActor* actor);
	

	void unregisterZOrderActor(SGActor* actor);
	void unregisterProjectile(SGProjectile* projectile);
	void unregisterCharacter(SGCharacter* chracter);
	void unregisterMonster(SGMonster* mosnter);
	void unregisterObstacle(SGObstacle* obstacle);
	void unregisterEffect(SGEffect* effect);
	void unregisterPhysicsActor(SGPhysicsActor* physicsActor);
	void unregisterActor(SGActor* actor);

	void cleanUpProjectile(SGProjectile* projectile);
	void cleanUpMonster(SGMonster* monster);
	void cleanUpObstacle(SGObstacle* obstacle);
	void cleanUpCharacter(SGCharacter* character);
	void cleanUpEffect(SGEffect* effect);

	ActorList& getZOrderActorList() { return m_vZOrderedActors; }
	ProjectileList& getProjectileList() { return m_vProjectiles; }
	MonsterList& getMonsterList() { return m_vMonsters; }
	ObstacleList& getCollidableObstacleList() { return m_vCollidableObstacles; }
	CharacterList& getCharacterList() { return m_vCharacters; }
	PhysicsActorList& getPhysicsActorList() { return m_vPhysicsActors; }
	SGActor* getActor(int actorId) { return m_hActorMap[actorId]; }
private:
	// 풀링용 리스트
	SGHashMap<int, SGList<SGMonster*>> m_hMonsterPool;
	SGHashMap<int, SGList<SGObstacle*>> m_hObstaclePool;
	SGHashMap<int, SGList<SGProjectile*>> m_hProjectilePool;
	SGHashMap<int, SGList<SGEffect*>> m_hEffectPool;

	// TODO: 캐릭터 풀링 - 이건 구현할려면 좀 고민해야함.
	// 애니메이션 프레임 텍스쳐중에 교체 가능한것들은 다 해줘야하기때문
	// 그외의 것들은 애니메이션이나 텍스쳐 변할일이 없기때문에 쉽게 풀링가능
	// 어차피 별로 없을 건데 풀링이 필요하진 않다.
	
	// 관리용 리스트
	SGHashMap<int, SGActor*> m_hActorMap;
	ActorList m_vZOrderedActors;				// Z오더를 고유 ID처럼 사용해도 될듯? 이진탐색으로 삭제 함 댈것같다.
	ProjectileList m_vProjectiles;		// 플레이어가 만든 충돌체
	MonsterList m_vMonsters;
	ObstacleList m_vCollidableObstacles;
	CharacterList m_vCharacters;
	PhysicsActorList m_vPhysicsActors;
	EffectList m_vEffectList;

	SGHashMap<SGActor*, SGActor*> m_hRemoveActorMap;	// 디버깅용, 중복 큐잉 방지용
	SGArrayQueue<SGActor*> m_qRemovedActors;

	SGMapLayer* m_pMapLayer;

	bool m_bCleared;
	float m_fZReorderTime;
	int m_iIdSequence;
};




