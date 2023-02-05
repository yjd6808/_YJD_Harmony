/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 6:09:56 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

#include <SteinsGate/Research/SGStruct.h>
#include <SteinsGate/Research/SGProjectile.h>
#include <SteinsGate/Research/SGMonster.h>
#include <SteinsGate/Research/SGObstacle.h>
#include <SteinsGate/Research/SGCharacter.h>
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

	static SGActorBox* get() {
		static SGActorBox* s_pInstance = nullptr;

		if (s_pInstance == nullptr) {
			s_pInstance = new SGActorBox;
		}
		return s_pInstance;
	}

	void init(SGMapLayer* mapLayer);
	
	void update(float dt);
	
	void updateActors(float dt);
	void updateZOrder();
	int updateCleanUp();

	void clearRoom();
	void clearAll();

	// 캐릭터는 액터 박스가 생성/소멸을 관리하는 객체가 아니다.
	// 단지 생성만 도와줌
	SGCharacter* createCharacter(CharType_t charType, float x, float y, SGCharacterInfo& info);

	SGProjectile* createProejctileOnMap(SGActor* spawner, int projectileId);
	SGMonster* createMonsterOnMap(int monsterCode, int aiCode, float x, float y);
	SGObstacle* createObstacleOnMap(int obstacleCode, float x, float y);

	// spawner 캔버스 기준 상대좌표로 이펙트를 만든다.
	SGEffect* createEffectOnMapBySpawner(SGActor* spawner, int effectCode, float offsetX, float offsetY);
	SGEffect* createEffectOnMapBySpawner(SGActor* spawner, int effectCode, const SGVec2& offset);

	// 이펙트를 Attcher에게 붙인다.
	SGEffect* createEffectOnMapByAttacher(SGActor* attacher, int effectCode, float offsetX, float offsetY);
	SGEffect* createEffectOnMapByAttacher(SGActor* attacher, int effectCode, const SGVec2& offset);

	// 절대적인 위치에 이펙트 생성
	SGEffect* createEffectOnMapAbsolute(int effectCode, SpriteDirection_t direction, float x, float y, int zOrder);
	SGEffect* createEffectOnMapAbsolute(int effectCode, SpriteDirection_t direction, const SGVec2& pos, int zOrder);
	SGEffect* createEffectOnMapAbsolute(int effectCode, float x, float y, int zOrder);
	SGEffect* createEffectOnMapAbsolute(int effectCode, const SGVec2& pos, int zOrder);
	

	// 대상과 충돌한 지점에 이펙트 생성
	SGEffect* createEffectOnMapTargetCollision(int effectCode, SpriteDirection_t direction, const SGHitInfo& info, bool randomRotation = false);
	SGEffect* createEffectOnMapTargetCollision(int effectCode, const SGHitInfo& info, bool randomRotation = false);
	SGEffect* createEffectOnMapTargetCollision(int effectCode, const SGHitInfo& info, float offsetX, float offsetY, bool randomRotation = false);
	SGEffect* createEffectOnMapTargetCollision(int effectCode, const SGHitInfo& info, const SGVec2& offset, bool randomRotation = false);

	// 일단 생성
	SGEffect* createEffectOnMap(int effectCode);

	void sortZOrderActor();
	void registerCleanUp(SGActor* actor);
	void unregisterZOrderActor(SGActor* actor);

	void registerProjectile(SGProjectile* projectile);
	void registerCharacter(SGCharacter* character);
	void registerMonster(SGMonster* mosnter);
	void registerObstacle(SGObstacle* obstacle);
	void registerEffect(SGEffect* effect);
	void registerActor(SGActor* actor);

	void cleanUpProjectile(SGProjectile* projectile);
	void cleanUpMonster(SGMonster* monster);
	void cleanUpObstacle(SGObstacle* obstacle);
	void cleanUpCharacter(SGCharacter* character);
	void cleanUpEffect(SGEffect* effect);

	void unregisterProjectile(SGProjectile* projectile);
	void unregisterCharacter(SGCharacter* chracter);
	void unregisterMonster(SGMonster* mosnter);
	void unregisterObstacle(SGObstacle* obstacle);
	void unregisterEffect(SGEffect* effect);
	void unregisterColidableObstacle(SGObstacle* obstacle);
	void unregisterPhysicsActor(SGPhysicsActor* physicsActor);
	void unregisterActor(SGActor* actor);

	

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
	// 캐릭터 말고는 애니메이션이나 파츠 텍스쳐들이 변할일이 없기때문에 쉽게 풀링가능
	
	// 관리용 리스트
	SGHashMap<int, SGActor*> m_hActorMap;
	ActorList m_vZOrderedActors;		
	ProjectileList m_vProjectiles;		
	MonsterList m_vMonsters;
	ObstacleList m_vCollidableObstacles;
	ObstacleList m_vObstacles;
	CharacterList m_vCharacters;
	PhysicsActorList m_vPhysicsActors;
	EffectList m_vEffectList;

	SGHashMap<SGActor*, SGActor*> m_hRemoveActorMap;	// 중복 큐잉 방지용
	SGArrayQueue<SGActor*> m_qRemovedActors;

	SGMapLayer* m_pMapLayer;

	bool m_bCleared;
	float m_fZReorderTime;
	int m_iIdSequence;
};




