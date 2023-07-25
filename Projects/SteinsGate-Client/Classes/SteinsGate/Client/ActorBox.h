/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 6:09:56 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

#include <SteinsGate/Client/Struct.h>
#include <SteinsGate/Client/Projectile.h>
#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/MapObject.h>
#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Effect.h>

using ActorList = SGVector<Actor*>;
using CharacterList = SGVector<Character*>;
using ProjectileList = SGVector<Projectile*>;
using MonsterList = SGVector<Monster*>;
using PhysicsActorList = SGVector<PhysicsActor*>;
using MapObjectList = SGVector<MapObject*>;
using EffectList = SGVector<Effect*>;

class MapLayer;
class ActorBox final : public JCore::SingletonPointer<ActorBox>
{
private:
	friend class TSingleton;
	ActorBox();
	~ActorBox() override;
public:
	void init(MapLayer* mapLayer);
	
	void update(float dt);
	
	void updateActors(float dt);
	void updateZOrder();
	int updateCleanUp();

	void clearRoom();
	void clearAll();

	// 캐릭터는 액터 박스가 생성/소멸을 관리하는 객체가 아니다.
	// 단지 생성만 도와줌
	Character* createCharacterOnMap(CharType_t charType, float x, float y, VisualInfo& info);
	Projectile* createProejctileOnMap(Actor* spawner, int projectileId);
	Monster* createMonsterOnMap(int monsterCode, int aiCode, float x, float y);
	MapObject* createMapObjectOnMap(int mapObjectCode, float x, float y);
	void registerPlayerOnMap(Player* player);

	// spawner 캔버스 기준 상대좌표로 이펙트를 만든다.
	Effect* createEffectOnMapBySpawner(Actor* spawner, int effectCode, float offsetX, float offsetY);
	Effect* createEffectOnMapBySpawner(Actor* spawner, int effectCode, const SGVec2& offset);

	// 이펙트를 Attcher에게 붙인다.
	Effect* createEffectOnMapByAttacher(Actor* attacher, int effectCode, float offsetX, float offsetY);
	Effect* createEffectOnMapByAttacher(Actor* attacher, int effectCode, const SGVec2& offset);

	// 절대적인 위치에 이펙트 생성
	Effect* createEffectOnMapAbsolute(int effectCode, SpriteDirection_t direction, float x, float y, int zOrder);
	Effect* createEffectOnMapAbsolute(int effectCode, SpriteDirection_t direction, const SGVec2& pos, int zOrder);
	Effect* createEffectOnMapAbsolute(int effectCode, float x, float y, int zOrder);
	Effect* createEffectOnMapAbsolute(int effectCode, const SGVec2& pos, int zOrder);
	

	// 대상과 충돌한 지점에 이펙트 생성
	Effect* createEffectOnMapTargetCollision(int effectCode, SpriteDirection_t direction, const HitInfo& info, bool randomRotation = false);
	Effect* createEffectOnMapTargetCollision(int effectCode, const HitInfo& info, bool randomRotation = false);
	Effect* createEffectOnMapTargetCollision(int effectCode, const HitInfo& info, float offsetX, float offsetY, bool randomRotation = false);
	Effect* createEffectOnMapTargetCollision(int effectCode, const HitInfo& info, const SGVec2& offset, bool randomRotation = false);

	// 일단 생성
	Effect* createEffectOnMap(int effectCode);

	void cleanUpAtNextFrame(Actor* actor);
	void cleanUp(Actor* actor);

	ActorList& getZOrderActorList() { return m_vZOrderedActors; }
	ProjectileList& getProjectileList() { return m_vProjectiles; }
	MonsterList& getMonsterList() { return m_vMonsters; }
	MapObjectList& getCollidableMapObjectList() { return m_vCollidableMapObjects; }
	CharacterList& getCharacterList() { return m_vCharacters; }
	PhysicsActorList& getPhysicsActorList() { return m_vPhysicsActors; }
	Actor* getActor(int actorId) { return m_hActorMap[actorId]; }
private:
	void sortZOrderActor();
	void unregisterZOrderActor(Actor* actor);

	void cleanUpProjectile(Projectile* projectile);
	void cleanUpMonster(Monster* monster);
	void cleanUpMapObject(MapObject* mapObject);
	void cleanUpCharacter(Character* character);
	void cleanUpEffect(Effect* effect);

	void registerProjectile(Projectile* projectile);
	void registerCharacter(Character* character);
	void registerMonster(Monster* mosnter);
	void registerMapObject(MapObject* mapObject);
	void registerEffect(Effect* effect);
	void registerActor(Actor* actor);

	void unregisterProjectile(Projectile* projectile);
	void unregisterCharacter(Character* chracter);
	void unregisterMonster(Monster* mosnter);
	void unregisterMapObject(MapObject* mapObject);
	void unregisterEffect(Effect* effect);
	void unregisterColidableMapObject(MapObject* mapObject);
	void unregisterPhysicsActor(PhysicsActor* physicsActor);
	void unregisterActor(Actor* actor);
private:
	// 풀링용 리스트
	SGHashMap<int, SGLinkedList<Monster*>> m_hMonsterPool;
	SGHashMap<int, SGLinkedList<MapObject*>> m_hMapObjectPool;
	SGHashMap<int, SGLinkedList<Projectile*>> m_hProjectilePool;
	SGHashMap<int, SGLinkedList<Effect*>> m_hEffectPool;

	// TODO: 캐릭터 풀링 - 이건 구현할려면 좀 고민해야함.
	// 애니메이션 프레임 텍스쳐중에 교체 가능한것들은 다 해줘야하기때문
	// 캐릭터 말고는 애니메이션이나 파츠 텍스쳐들이 변할일이 없기때문에 쉽게 풀링가능
	
	// 관리용 리스트
	SGHashMap<int, Actor*> m_hActorMap;
	ActorList m_vZOrderedActors;		
	ProjectileList m_vProjectiles;		
	MonsterList m_vMonsters;
	MapObjectList m_vCollidableMapObjects;
	MapObjectList m_vMapObjects;
	CharacterList m_vCharacters;
	PhysicsActorList m_vPhysicsActors;
	EffectList m_vEffectList;

	SGHashMap<Actor*, Actor*> m_hRemoveActorMap;	// 중복 큐잉 방지용
	SGArrayQueue<Actor*> m_qRemovedActors;

	MapLayer* m_pMapLayer;

	bool m_bCleared;
	float m_fZReorderTime;
	int m_iIdSequence;
};




