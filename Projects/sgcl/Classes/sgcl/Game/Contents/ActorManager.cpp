/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 6:10:07 PM
 * =====================
 *
 * 1. 캐릭터의 경우 외부 플레이어가 관리함
 * 2. 던전 단위, 마을 단위로 clean
 * 3.  던전, 마을 단위 이동시에는 풀까지 전부 비우기
 *     방단위 이동시에는 전부 풀로 복귀
 *
 * 전체 정리 순서
 */

#include "Game/Contents/ActorManager.h"

#include "sg/Util/DescLoaderMgr.h"

#include "sgcl/Struct/CL_Character.h"

#include "sgcl/Game/Contents/ActorListenerManager.h"
#include "sgcl/Layer/Layer_Map.h"
#include "sgcl/Game/Component/AIComponent.h"

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ActorManager::ActorManager()
: pMapLayer_(nullptr)
, isCleared_(true)
, zReorderTime_(0.0f)
, idSequence_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorManager::~ActorManager()
{
	ClearAll();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::Init(MapLayer* _pMapLayer)
{
	jc_assert_msg(isCleared_ == true, "먼저 깔끔하게 비워주세요.");

	idSequence_ = 0;

	pMapLayer_ = _pMapLayer;
	pMapLayer_->retain();
	isCleared_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename TActor>
static int ReleasePool(jc::HashMap<int, jc::LinkedList<TActor*>>& _pool)
{
	int releaseCount = 0;
	_pool.ForEachKey([&releaseCount , &_pool](int& _code)
	{
		jc::LinkedList<TActor*>& li = _pool[_code];
		auto it = li.Begin();
		while (it.HasNext())
		{
			TActor* pActor = it.Next();
			++releaseCount;
			CC_SAFE_RELEASE(pActor);
		}
	});
	_pool.Clear();
	return releaseCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename TActor>
static int ReleaseList(jc::Vector<TActor*>& _actorList)
{
	int releaseCount = 0;
	_actorList.ForEach([&releaseCount](TActor* _pActor)
	{
		CC_SAFE_RELEASE(_pActor);
		++releaseCount;
	});
	_actorList.Clear();
	_LogDebug_("[%10s] 릴리즈: %d개", typeid(TActor).name(), releaseCount);
	return releaseCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename TActor>
static int CleanUpList(jc::Vector<TActor*>& _actorList)
{
	int cleanUpCount = 0;
	_actorList.ForEach([&cleanUpCount](TActor* _pActor)
	{
		_pActor->CleanUpAtNextFrame();
		++cleanUpCount;
	});
	_actorList.Clear();
	_LogDebug_("[%10s] 클린업: %d개", typeid(TActor).name(), cleanUpCount);
	return cleanUpCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::ClearAll()
{
	if (isCleared_)
		return;

	// 캐릭터 제외하고 모두 릴리즈 수행
	jc_assert_msg(pMapLayer_, "[SGActorBox] 맵 레이어가 존재하지 않습니다.");

	// Step 1. 맵에 로딩되지 않은 풀의 엑터들을 정리해줘야한다.
	// Step 2. 맵에 로딩된 풀에 없는 엑터들을 정리해줘야한다.

	//cleanUpList<SGCharacter>(m_vCharacters);
	_LogDebug_("Step Before. 정리 대기중 엑터 수: %d개", removeActorMap_.Size());
	UpdateCleanUp();
	jc_assert_msg(removeActorMap_.Size() == 0, "아직 반환안된 액터가 있습니다. 이러면 안됩니다.");

	int step1ReleaseCount = 0;
	int step2ReleaseCount = 0;

	// Step1. 현재 맵에 로딩된 액터들 릴리즈
	step1ReleaseCount += ReleaseList<Character>(characterList_);
	step1ReleaseCount += ReleaseList<Projectile>(projectiles_);
	step1ReleaseCount += ReleaseList<Monster>(monsterList_);
	step1ReleaseCount += ReleaseList<MapObject>(mapObjectList_);
	step1ReleaseCount += ReleaseList<Effect>(effectList_);
	_LogDebug_("정리된 로디드 액터 수: %d개", step1ReleaseCount);
	jc_assert_msg(step1ReleaseCount == actorMap_.Size(), "Step1. 실패");


	// Step2. 현재 맵에 로딩되지 않은 엑터들 릴리즈
	step2ReleaseCount += ReleasePool<Effect>(effectPool_);
	step2ReleaseCount += ReleasePool<Monster>(monsterPool_);
	step2ReleaseCount += ReleasePool<Projectile>(projectilePool_);
	step2ReleaseCount += ReleasePool<MapObject>(mapObjectPool_);
	_LogDebug_("정리된 언로디드 액터 수: %d개", step2ReleaseCount);

	zOrderedActorList_.Clear();
	collidableMapObjectList_.Clear();
	physicsActors_.Clear();
	actorMap_.Clear();

	auto pMap = pMapLayer_;
	CC_SAFE_RELEASE_NULL(pMapLayer_);
	isCleared_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::ClearRoom()
{
	// 여기선 해줘야함 삭제시키는게 목적이 아니라 풀로 복귀 시키는게 목적이기 때문
	int cleanUpCount = 0;

	cleanUpCount += CleanUpList<Projectile>(projectiles_);
	cleanUpCount += CleanUpList<Monster>(monsterList_);
	cleanUpCount += CleanUpList<MapObject>(mapObjectList_);
	cleanUpCount += CleanUpList<Effect>(effectList_);
	cleanUpCount += CleanUpList<Character>(characterList_);

	_LogDebug_("Step Before. 정리 대기중 엑터 수: %d개", removeActorMap_.Size());
	if (UpdateCleanUp() != cleanUpCount)
	{
		jc_assert_msg(false, "정리가 제대로 수행되지 않았습니다.");
	}
	jc_assert_msg(removeActorMap_.Size() == 0, "아직 반환안된 액터가 있습니다. 이러면 안됩니다.");

	zOrderedActorList_.Clear();
	collidableMapObjectList_.Clear();
	physicsActors_.Clear();
	actorMap_.Clear();

	// 풀 4개는 건들지 않음

	CC_SAFE_RELEASE_NULL(pMapLayer_);
	isCleared_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::Update(float _dt)
{
	if (isCleared_)
		return;

	jc_assert_msg(pMapLayer_, "[SGActorBox] 맵 레이어가 세팅되지 않았습니다.");

	UpdateZOrder();
	UpdateActors(_dt);
	UpdateCleanUp();
}

//////////////////////////////////////////////////////////////////////////////////////////
int ActorManager::UpdateCleanUp()
{
	int iCleanUpCount = removedActorQueue_.Size();
	while (!removedActorQueue_.IsEmpty())
	{
		Actor* pRemovedActor = removedActorQueue_.Front();
		removedActorQueue_.Dequeue();
		removeActorMap_.Remove(pRemovedActor);
		CleanUp(pRemovedActor);
		pMapLayer_->removeChild(pRemovedActor);
	}

	jc_assert_msg(removeActorMap_.IsEmpty(), "아직 삭제되지 않은 액터가 있습니다. 말도 안됩니다.");
	return iCleanUpCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UpdateZOrder()
{
	// 틱 이전, 이후 비교했을때 높은 확률로 대부분 정렬되어 있으므로 삽입 정렬 진행
	SortZOrderActor();

	for (int iOrder = 1; iOrder <= zOrderedActorList_.Size(); ++iOrder)
	{
		pMapLayer_->reorderChild(zOrderedActorList_[iOrder - 1], iOrder);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UpdateActors(float _dt)
{
	for (int i = 0; i < projectiles_.Size(); ++i)
	{
		projectiles_[i]->update(_dt);
	}

	for (int i = 0; i < monsterList_.Size(); ++i)
	{
		monsterList_[i]->update(_dt);
	}

	for (int i = 0; i < mapObjectList_.Size(); ++i)
	{
		mapObjectList_[i]->update(_dt);
	}

	for (int i = 0; i < characterList_.Size(); ++i)
	{
		characterList_[i]->update(_dt);
	}

	for (int i = 0; i < effectList_.Size(); ++i)
	{
		effectList_[i]->update(_dt);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
Character* ActorManager::CreateCharacterOnMap(CharType_t _charType, float _x, float _y, VisualInfo& _visualInfo)
{
	jc_assert_msg(pMapLayer_, "맵 레이어 생성 및 init 후 캐릭터를 생성해주세요");

	CharBaseInfo* pCharInfo = g_cDescMgr.GetCharInfo(_charType);
	Character* pCharacter = Character::Create(pCharInfo, _visualInfo);

	pCharacter->SetPositionRealCenter(_x, _y);
	pCharacter->retain();
	pCharacter->SetAllyFlag(0);
	pCharacter->SetMapLayer(pMapLayer_);
	pCharacter->GetListenerCollection().OnCreated();

	RegisterCharacter(pCharacter);
	pMapLayer_->addChild(pCharacter);
	return pCharacter;
}

//////////////////////////////////////////////////////////////////////////////////////////
Projectile* ActorManager::CreateProejctileOnMap(Actor* _pSpawner, int _projectileId)
{
	jc_assert_msg(pMapLayer_, "맵 레이어 생성 및 init 후 프로젝틸을 생성해주세요");

	ProjectileInfo* pInfo = g_cDescMgr.GetProjectileInfo(_pSpawner->GetType(), _projectileId);


	if (!projectilePool_.Exist(_projectileId))
	{
		projectilePool_.Insert(Move(_projectileId), jc::LinkedList<Projectile*>());
	}

	Projectile* pProjectile = nullptr;
	jc::LinkedList<Projectile*>& projectileList = projectilePool_[_projectileId];

	if (projectileList.IsEmpty())
	{
		pProjectile = Projectile::Create(pInfo, _pSpawner);
		pProjectile->retain();
	}
	else
	{
		pProjectile = projectileList.Front();
		pProjectile->SetSpawner(_pSpawner); // 위치 세팅전 스포너 세팅 먼저 해줘야함
		pProjectile->Initialize();
		projectileList.PopFront();
	}

	pProjectile->GetListenerCollection().OnCreated();
	pProjectile->RunAnimation(1);
	pProjectile->SetAllyFlag(_pSpawner->GetAllyFlag());
	pProjectile->SetMapLayer(pMapLayer_);

	if (pInfo->spawnEffect_->code_ != InvalidValue_v)
	{
		Effect* pSpawnEffect = CreateEffectOnMapBySpawner(_pSpawner, pInfo->spawnEffect_->code_, pInfo->spawnEffectOffsetX_,
		                                                  pInfo->spawnEffectOffsetY_);
		pSpawnEffect->setLocalZOrder(_pSpawner->getLocalZOrder() + 1);
	}

	RegisterProjectile(pProjectile);
	pMapLayer_->addChild(pProjectile);
	return pProjectile;
}

//////////////////////////////////////////////////////////////////////////////////////////
Monster* ActorManager::CreateMonsterOnMap(int _monsterCode, int _aiCode, float _x, float _y)
{
	jc_assert_msg(pMapLayer_, "맵 레이어 생성 및 init 후 몬스터를 생성해주세요");

	MonsterInfo* pMonsterInfo = g_cDescMgr.GetMonsterInfo(_monsterCode);
	AIInfo* pAIInfo = g_cDescMgr.GetAiInfo(_aiCode);

	if (!monsterPool_.Exist(_monsterCode))
	{
		monsterPool_.Insert(Move(_monsterCode), jc::LinkedList<Monster*>());
	}

	Monster* pMonster = nullptr;
	jc::LinkedList<Monster*>& monsterList = monsterPool_[_monsterCode];

	if (monsterList.IsEmpty())
	{
		pMonster = Monster::Create(pMonsterInfo);
		pMonster->retain();
	}
	else
	{
		pMonster = monsterList.Front();
		pMonster->Initialize();
		monsterList.PopFront();
	}

	if (!pMonster->HasComponent(IComponent::eAI))
	{
		AIComponent* pAIComponent = dbg_new AIComponent(pMonster);
		pAIComponent->SetAiInfo(pAIInfo);
		pMonster->AddComponent(pAIComponent);
	}

	pMonster->GetListenerCollection().OnCreated();
	pMonster->SetPositionReal(_x, _y);
	pMonster->SetMapLayer(pMapLayer_);
	pMonster->SetAllyFlag(1);

	RegisterMonster(pMonster);
	pMapLayer_->addChild(pMonster);

	return pMonster;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapObject* ActorManager::CreateMapObjectOnMap(int _mapObjectCode, float _x, float _y)
{
	jc_assert_msg(pMapLayer_, "맵 레이어 생성 및 init 후 옵스터클을 생성해주세요");

	MapObjectInfo* pMapObjectInfo = g_cDescMgr.GetMapObjectInfo(_mapObjectCode);

	if (!mapObjectPool_.Exist(_mapObjectCode))
	{
		mapObjectPool_.Insert(Move(_mapObjectCode), jc::LinkedList<MapObject*>());
	}

	MapObject* pMapObject = nullptr;
	jc::LinkedList<MapObject*>& mapObjectList = mapObjectPool_[_mapObjectCode];

	if (mapObjectList.IsEmpty())
	{
		pMapObject = MapObject::Create(pMapObjectInfo);
		pMapObject->retain();
	}
	else
	{
		pMapObject = mapObjectList.Front();
		pMapObject->Initialize();
		mapObjectList.PopFront();
	}

	pMapObject->GetListenerCollection().OnCreated();
	pMapObject->SetPositionReal(_x, _y);
	pMapObject->SetMapLayer(pMapLayer_);

	RegisterMapObject(pMapObject);
	pMapLayer_->addChild(pMapObject);

	return pMapObject;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::RegisterPlayerOnMap(Player* _pPlayer)
{
	for (int i = 0; i < characterList_.Size(); ++i)
	{
		if (characterList_[i] == _pPlayer)
		{
			jc_assert_msg(false, "이미 캐릭터가 맵에 포함되어 있습니다.");
			return;
		}
	}

	RegisterCharacter(_pPlayer);
	_pPlayer->retain();
	pMapLayer_->addChild(_pPlayer);
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapBySpawner(Actor* _pSpawner, int _effectCode, float _offsetX, float _offsetY)
{
	Effect* pEffect = CreateEffectOnMap(_effectCode);

	cc::size spawnerCanvsSize = _pSpawner->GetCanvasSize();
	cc::vec2 spawnerCanvasPos = _pSpawner->GetCanvasPositionReal();

	pEffect->SetSpriteDirection(_pSpawner->GetSpriteDirection());

	if (_pSpawner->GetSpriteDirection() == SpriteDirection::Right)
	{
		pEffect->SetPositionReal(
			spawnerCanvasPos.x + _offsetX,
			spawnerCanvasPos.y + _offsetY
		);
	}
	else
	{
		pEffect->SetPositionReal(
			spawnerCanvasPos.x + spawnerCanvsSize.width - _offsetX,
			spawnerCanvasPos.y + _offsetY
		);
	}

	return pEffect;
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapBySpawner(Actor* _pSpawner, int _effectCode, const cc::vec2& _offset)
{
	return CreateEffectOnMapBySpawner(_pSpawner, _effectCode, _offset.x, _offset.y);
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapAbsolute(int _effectCode, SpriteDirection_t _direction, float _x, float _y, int _zOrder)
{
	Effect* pEffect = CreateEffectOnMap(_effectCode);
	pEffect->SetSpriteDirection(_direction);
	pEffect->SetPositionRealCenter(_x, _y);
	pEffect->setLocalZOrder(_zOrder);
	return pEffect;
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapAbsolute(int _effectCode, SpriteDirection_t _direction, const cc::vec2& _pos, int _zOrder)
{
	return CreateEffectOnMapAbsolute(_effectCode, _direction, _pos.x, _pos.y, _zOrder);
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapAbsolute(int _effectCode, float _x, float _y, int _zOrder)
{
	Effect* pEffect = CreateEffectOnMap(_effectCode);
	pEffect->SetPositionRealCenter(_x, _y);
	pEffect->setLocalZOrder(_zOrder);
	return pEffect;
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapAbsolute(int _effectCode, const cc::vec2& _pos, int _zOrder)
{
	return CreateEffectOnMapAbsolute(_effectCode, _pos.x, _pos.y, _zOrder);
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapTargetCollision(int _effectCode, SpriteDirection_t _direction, const HitInfo& _info, bool _randomRotation)
{
	Effect* pEffect = CreateEffectOnMapAbsolute(_effectCode, _info.hitRect_.getMid(),
	                                            _info.pHitTarget_->getLocalZOrder() + 1);
	pEffect->SetSpriteDirection(_direction);

	if (_randomRotation)
	{
		pEffect->setRotation(g_cRandom.GenerateF(0.0f, 360.0f));
	}

	return pEffect;
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapTargetCollision(int _effectCode, const HitInfo& _info, bool _randomRotation)
{
	return CreateEffectOnMapTargetCollision(_effectCode, SpriteDirection::Right, _info, _randomRotation);
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapTargetCollision(int _effectCode, const HitInfo& _info, float _offsetX, float _offsetY, bool _randomRotation)
{
	Effect* pEffect = CreateEffectOnMapTargetCollision(_effectCode, _info, _randomRotation);
	pEffect->setPosition(pEffect->getPositionX() + _offsetX, pEffect->getPositionX() + _offsetY);
	return pEffect;
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMapTargetCollision(int _effectCode, const HitInfo& _info, const cc::vec2& _offset, bool _randomRotation)
{
	return CreateEffectOnMapTargetCollision(_effectCode, _info, _offset.x, _offset.y, _randomRotation);
}

//////////////////////////////////////////////////////////////////////////////////////////
Effect* ActorManager::CreateEffectOnMap(int _effectCode)
{
	jc_assert_msg(pMapLayer_, "맵 레이어 생성 및 init 후 이펙트를 생성해주세요");

	EffectInfo* pEffectInfo = g_cDescMgr.GetEffectInfo(_effectCode);

	if (!effectPool_.Exist(_effectCode))
	{
		effectPool_.Insert(Move(_effectCode), jc::LinkedList<Effect*>());
	}

	Effect* pEffect = nullptr;
	jc::LinkedList<Effect*>& effectList = effectPool_[_effectCode];

	if (effectList.IsEmpty())
	{
		pEffect = Effect::Create(pEffectInfo);
		pEffect->retain();
	}
	else
	{
		pEffect = effectList.Front();
		pEffect->InitVariables();
		effectList.PopFront();
	}

	pEffect->RunAnimation(1);
	pEffect->InitThicknessBox({ 0, 0, 0, 0 });
	pEffect->SetMapLayer(pMapLayer_);
	RegisterEffect(pEffect);
	pMapLayer_->addChild(pEffect);
	return pEffect;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::RegisterCharacter(Character* _pChar)
{
	characterList_.PushBack(_pChar);
	physicsActors_.PushBack(_pChar);
	zOrderedActorList_.PushBack(_pChar);
	RegisterActor(_pChar);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::RegisterProjectile(Projectile* _pProjectile)
{
	projectiles_.PushBack(_pProjectile);
	zOrderedActorList_.PushBack(_pProjectile);
	RegisterActor(_pProjectile);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::RegisterMonster(Monster* _pMonster)
{
	monsterList_.PushBack(_pMonster);
	zOrderedActorList_.PushBack(_pMonster);
	physicsActors_.PushBack(_pMonster);
	RegisterActor(_pMonster);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::RegisterMapObject(MapObject* _pMapObject)
{
	mapObjectList_.PushBack(_pMapObject);

	switch (_pMapObject->GetObjectType())
	{
	case MapObjectType::Obstacle:
		{
			MapObjectObstacleInfo* pInfo = dynamic_cast<MapObjectObstacleInfo*>(_pMapObject->GetBaseInfo());

			if (pInfo->isZOrederable_)
				zOrderedActorList_.PushBack(_pMapObject);
			if (pInfo->isColliadalble_)
				collidableMapObjectList_.PushBack(_pMapObject);

			break;
		}
	case MapObjectType::Gate:
		{
			zOrderedActorList_.PushBack(_pMapObject);
			break;
		}
	}

	RegisterActor(_pMapObject);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::RegisterEffect(Effect* _pEffect)
{
	effectList_.PushBack(_pEffect);
	RegisterActor(_pEffect);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::RegisterActor(Actor* _pActor)
{
	int uniqueId = ++idSequence_;
	_pActor->SetActorId(uniqueId);
	if (actorMap_.Insert(uniqueId, _pActor) == false)
	{
		jc_assert_msg(false, "이미 %s 액터가 ActorMap에 등록되어 있습니다. (코드: %d)", _pActor->GetTypeName(), uniqueId);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::CleanUp(Actor* _pActor)
{
	switch (_pActor->GetType())
	{
	case ActorType::Character: CleanUpCharacter(dynamic_cast<Character*>(_pActor));
		break;
	case ActorType::Monster: CleanUpMonster(dynamic_cast<Monster*>(_pActor));
		break;
	// case ActorType::Npc: cleanUpNpc(dynamic_cast<Character*>(actor)); break;
	case ActorType::Projectile: CleanUpProjectile(dynamic_cast<Projectile*>(_pActor));
		break;
	case ActorType::MapObject: CleanUpMapObject(dynamic_cast<MapObject*>(_pActor));
		break;
	case ActorType::Effect: CleanUpEffect(dynamic_cast<Effect*>(_pActor));
		break;
	default: jc_assert(false);
	}

	_pActor->GetListenerCollection().OnCleanUp();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::SortZOrderActor()
{
	zOrderedActorList_.InsertionSort([](Actor* _lhs, Actor* _rhs)
	{
		return _lhs->GetThicknessBoxRect().getMidY() > _rhs->GetThicknessBoxRect().getMidY();
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::CleanUpAtNextFrame(Actor* _pActor)
{
	// 이미 존재하는 경우 무시
	if (removeActorMap_.Exist(_pActor))
	{
		return;
	}
	removeActorMap_.Insert(_pActor, _pActor);
	removedActorQueue_.Enqueue(_pActor);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UnregisterZOrderActor(Actor* _pActor)
{
	int iOffset = zOrderedActorList_.OffsetLowerBound(_pActor->getLocalZOrder(), [](const Actor* _actor, int _zorder)
	{
		return _actor->getLocalZOrder() < _zorder;
	});

	jc_assert_msg(iOffset != -1, "Z 오더 목록에서 대상을 찾지 못했습니다.");
	jc_assert_msg(zOrderedActorList_[iOffset] == _pActor, "Z 오더 목록에서 찾은 대상이 동일하지 않습니다. 로직 오류! 로직 오류!");

	if (zOrderedActorList_[iOffset] == _pActor)
	{
		zOrderedActorList_.RemoveAt(iOffset);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UnregisterProjectile(Projectile* _pProjectile)
{
	if (!projectiles_.Remove(_pProjectile))
	{
		jc_assert_msg(false, "프로젝틸 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UnregisterCharacter(Character* _pChar)
{
	if (!characterList_.Remove(_pChar))
	{
		jc_assert_msg(false, "캐릭터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UnregisterMonster(Monster* _pMonster)
{
	if (!monsterList_.Remove(_pMonster))
	{
		jc_assert_msg(false, "몬스터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UnregisterMapObject(MapObject* _pMapObject)
{
	if (!mapObjectList_.Remove(_pMapObject))
	{
		jc_assert_msg(false, "옵스터클 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UnregisterColidableMapObject(MapObject* _pMapObject)
{
	if (!collidableMapObjectList_.Remove(_pMapObject))
	{
		jc_assert_msg(false, "충돌 가능한 옵스터클 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UnregisterEffect(Effect* _pEffect)
{
	if (!effectList_.Remove(_pEffect))
		jc_assert_msg(false, "이펙트 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UnregisterPhysicsActor(Actor* _pPhysicsActor)
{
	if (!physicsActors_.Remove(_pPhysicsActor))
		jc_assert_msg(false, "피직스 액터 목록에서 액터를 제거하는데 실패했습니다.");
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::UnregisterActor(Actor* _pActor)
{
	jc_assert_msg(_pActor->GetActorId() != InvalidValue_v, "올바르지 않은 액터 ID입니다.");
	if (!actorMap_.Remove(_pActor->GetActorId()))
		jc_assert_msg(false, "공통 액터 목록에서 액터를 제거하는데 실패했습니다.");
	else
		_pActor->SetActorId(InvalidValue_v);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::CleanUpProjectile(Projectile* _pProjectile)
{
	UnregisterProjectile(_pProjectile);
	UnregisterZOrderActor(_pProjectile);
	UnregisterActor(_pProjectile);
	projectilePool_[_pProjectile->GetBaseInfo()->code_].PushBack(_pProjectile);
	_LogDebug_("삭제> 플레이어 프로젝틸 (%s), 남은 플레이어 프로젝틸 수 : %d, Z오더 액터 수: %d", _pProjectile->GetBaseInfo()->name_.Source(),
	           projectiles_.Size(), zOrderedActorList_.Size());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::CleanUpMonster(Monster* _pMonster)
{
	UnregisterMonster(_pMonster);
	UnregisterZOrderActor(_pMonster);

	if (_pMonster->HasComponent(IComponent::ePhysics))
		UnregisterPhysicsActor(_pMonster);

	UnregisterActor(_pMonster);
	monsterPool_[_pMonster->GetBaseInfo()->code_].PushBack(_pMonster);
	_LogDebug_("삭제> 몬스터 (%s), 남은 몬스터 수 : %d, Z오더 액터 수: %d", _pMonster->GetBaseInfo()->name_.Source(), monsterList_.Size(), zOrderedActorList_.Size());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::CleanUpMapObject(MapObject* _pMapObject)
{
	UnregisterMapObject(_pMapObject);
	UnregisterActor(_pMapObject);

	MapObjectInfo* pBaseInfo = _pMapObject->GetBaseInfo();

	switch (_pMapObject->GetObjectType())
	{
	case MapObjectType::Obstacle:
		{
			MapObjectObstacleInfo* pInfo = dynamic_cast<MapObjectObstacleInfo*>(_pMapObject->GetBaseInfo());

			if (pInfo->isZOrederable_)
				UnregisterZOrderActor(_pMapObject);
			if (pInfo->isColliadalble_)
				UnregisterColidableMapObject(_pMapObject);

			break;
		}
	case MapObjectType::Gate:
		{
			UnregisterZOrderActor(_pMapObject);
			break;
		}
	}

	mapObjectPool_[pBaseInfo->code_].PushBack(_pMapObject);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::CleanUpCharacter(Character* _pChar)
{
	UnregisterCharacter(_pChar);
	UnregisterZOrderActor(_pChar);
	UnregisterActor(_pChar);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorManager::CleanUpEffect(Effect* _pEffect)
{
	UnregisterEffect(_pEffect);
	UnregisterActor(_pEffect);

	effectPool_[_pEffect->GetBaseInfo()->code_].PushBack(_pEffect);
}
