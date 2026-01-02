/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 6:09:56 PM
 * =====================
 *
 */


#pragma once

#include <sgcl/Struct.h>
#include <sgcl/Projectile.h>
#include <sgcl/Monster.h>
#include <sgcl/MapObject.h>
#include <sgcl/HostPlayer.h>
#include <sgcl/Effect.h>

using ActorList = jc::Vector<Actor*>;
using CharacterList = jc::Vector<Character*>;
using ProjectileList = jc::Vector<Projectile*>;
using MonsterList = jc::Vector<Monster*>;
using MapObjectList = jc::Vector<MapObject*>;
using EffectList = jc::Vector<Effect*>;

class MapLayer;

class ActorManager final : public jc::SingletonPointer<ActorManager>
{
private:
	friend class TSingleton;
	ActorManager();
	~ActorManager();

public:
	void Init(MapLayer* _pMapLayer);

	void Update(float _dt);

	void UpdateActors(float _dt);
	void UpdateZOrder();
	int UpdateCleanUp();

	void ClearRoom();
	void ClearAll();

	// 캐릭터는 액터 박스가 생성/소멸을 관리하는 객체가 아니다.
	// 단지 생성만 도와줌
	Character*		CreateCharacterOnMap(CharType_t _charType, float _x, float _y, VisualInfo& _visualInfo);
	Projectile*		CreateProejctileOnMap(Actor* _pSpawner, int _projectileId);
	Monster*		CreateMonsterOnMap(int _monsterCode, int _aiCode, float _x, float _y);
	MapObject*		CreateMapObjectOnMap(int _mapObjectCode, float _x, float _y);
	void			RegisterPlayerOnMap(Player* _pPlayer);

	// spawner 캔버스 기준 상대좌표로 이펙트를 만든다.
	Effect* CreateEffectOnMapBySpawner(Actor* _pSpawner, int _effectCode, float _offsetX, float _offsetY);
	Effect* CreateEffectOnMapBySpawner(Actor* _pSpawner, int _effectCode, const cc::vec2& _offset);

	// 이펙트를 Attcher에게 붙인다.
	Effect* CreateEffectOnMapByAttacher(Actor* _pAttacher, int _effectCode, float _offsetX, float _offsetY);
	Effect* CreateEffectOnMapByAttacher(Actor* _pAttacher, int _effectCode, const cc::vec2& _offset);

	// 절대적인 위치에 이펙트 생성
	Effect* CreateEffectOnMapAbsolute(int _effectCode, SpriteDirection_t _direction, float _x, float _y, int _zOrder);
	Effect* CreateEffectOnMapAbsolute(int _effectCode, SpriteDirection_t _direction, const cc::vec2& _pos, int _zOrder);
	Effect* CreateEffectOnMapAbsolute(int _effectCode, float _x, float _y, int _zOrder);
	Effect* CreateEffectOnMapAbsolute(int _effectCode, const cc::vec2& _pos, int _zOrder);


	// 대상과 충돌한 지점에 이펙트 생성
	Effect* CreateEffectOnMapTargetCollision(int _effectCode, SpriteDirection_t _direction, const HitInfo& _info, bool _randomRotation = false);
	Effect* CreateEffectOnMapTargetCollision(int _effectCode, const HitInfo& _info, bool _randomRotation = false);
	Effect* CreateEffectOnMapTargetCollision(int _effectCode, const HitInfo& _info, float _offsetX, float _offsetY, bool _randomRotation = false);
	Effect* CreateEffectOnMapTargetCollision(int _effectCode, const HitInfo& _info, const cc::vec2& _offset, bool _randomRotation = false);

	// 일단 생성
	Effect* CreateEffectOnMap(int _effectCode);

	void CleanUpAtNextFrame(Actor* _pActor);
	void CleanUp(Actor* _pActor);

	ActorList&		GetZOrderActorList() { return zOrderedActorList_; }
	ProjectileList& GetProjectileList() { return projectiles_; }
	MonsterList&	GetMonsterList() { return monsterList_; }
	MapObjectList&	GetCollidableMapObjectList() { return collidableMapObjectList_; }
	CharacterList&	GetCharacterList() { return characterList_; }
	ActorList&		GetPhysicsActorList() { return physicsActors_; }
	Actor*			GetActor(int _actorId) { return actorMap_[_actorId]; }

private:
	void SortZOrderActor();
	void UnregisterZOrderActor(Actor* _pActor);

	void CleanUpProjectile(Projectile* _pProjectile);
	void CleanUpMonster(Monster* _pMonster);
	void CleanUpMapObject(MapObject* _pMapObject);
	void CleanUpCharacter(Character* _pChar);
	void CleanUpEffect(Effect* _pEffect);

	void RegisterProjectile(Projectile* _pProjectile);
	void RegisterCharacter(Character* _pChar);
	void RegisterMonster(Monster* _pMonster);
	void RegisterMapObject(MapObject* _pMapObject);
	void RegisterEffect(Effect* _pEffect);
	void RegisterActor(Actor* _pActor);

	void UnregisterProjectile(Projectile* _pProjectile);
	void UnregisterCharacter(Character* _pChar);
	void UnregisterMonster(Monster* _pMonster);
	void UnregisterMapObject(MapObject* _pMapObject);
	void UnregisterEffect(Effect* _pEffect);
	void UnregisterColidableMapObject(MapObject* _pMapObject);
	void UnregisterPhysicsActor(Actor* _pPhysicsActor);
	void UnregisterActor(Actor* _pActor);

private:
	// 풀링용 리스트
	jc::HashMap<int, jc::LinkedList<Monster*>> monsterPool_;
	jc::HashMap<int, jc::LinkedList<MapObject*>> mapObjectPool_;
	jc::HashMap<int, jc::LinkedList<Projectile*>> projectilePool_;
	jc::HashMap<int, jc::LinkedList<Effect*>> effectPool_;

	// TODO: 캐릭터 풀링 - 이건 구현할려면 좀 고민해야함.
	// 애니메이션 프레임 텍스쳐중에 교체 가능한것들은 다 해줘야하기때문
	// 캐릭터 말고는 애니메이션이나 파츠 텍스쳐들이 변할일이 없기때문에 쉽게 풀링가능

	// 관리용 리스트
	jc::HashMap<int, Actor*> actorMap_;
	ActorList zOrderedActorList_;
	ProjectileList projectiles_;
	MonsterList monsterList_;
	MapObjectList collidableMapObjectList_;
	MapObjectList mapObjectList_;
	CharacterList characterList_;
	ActorList physicsActors_;
	EffectList effectList_;

	jc::HashMap<Actor*, Actor*> removeActorMap_; // 중복 큐잉 방지용
	jc::ArrayQueue<Actor*> removedActorQueue_;

	MapLayer* pMapLayer_;

	bool isCleared_;
	float zReorderTime_;
	int idSequence_;
};
