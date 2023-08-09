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

#include "Tutturu.h"
#include "ActorBox.h"

#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/ActorListenerManager.h>
#include <SteinsGate/Client/MapLayer.h>

USING_NS_CC;
USING_NS_JC;

ActorBox::ActorBox()
	: m_iIdSequence(0)
	, m_pMapLayer(nullptr)
	, m_fZReorderTime(0.0f)
	, m_bCleared(true)
{}

ActorBox::~ActorBox() {
	clearAll();
}


void ActorBox::init(MapLayer* mapLayer) {
	DebugAssertMsg(m_bCleared == true, "먼저 깔끔하게 비워주세요.");

	m_iIdSequence = 0;

	m_pMapLayer = mapLayer;
	m_pMapLayer->retain();
	m_bCleared = false;
}

template <typename TActor>
static int releasePool(SGHashMap<int, SGLinkedList<TActor*>>& pool) {
	int iReleaseCount = 0;
	pool.ForEachKey([&iReleaseCount , &pool](int& code) {
		SGLinkedList<TActor*>& li = pool[code];
		auto it = li.Begin();
		while (it->HasNext()) {
			TActor* pActor = it->Next();
			++iReleaseCount;
			CC_SAFE_RELEASE(pActor);
		}
	});
	pool.Clear();
	return iReleaseCount;
}

template <typename TActor>
static int releaseList(SGVector<TActor*>& actorList) {
	int iReleaseCount = 0;
	actorList.ForEach([&iReleaseCount](TActor* actor) {
		CC_SAFE_RELEASE(actor);
		++iReleaseCount;	
	});
	actorList.Clear();
	_LogDebug_("[%10s] 릴리즈: %d개", typeid(TActor).name(), iReleaseCount);
	return iReleaseCount;
}

template <typename TActor>
static int cleanUpList(SGVector<TActor*>& actorList) {
	int iCleanUpCount = 0;
	actorList.ForEach([&iCleanUpCount](TActor* actor) {
		actor->cleanUpAtNextFrame();
		++iCleanUpCount;
	});
	actorList.Clear();
	_LogDebug_("[%10s] 클린업: %d개", typeid(TActor).name(), iCleanUpCount);
	return iCleanUpCount;
}

void ActorBox::clearAll() {

	if (m_bCleared)
		return;

	// 캐릭터 제외하고 모두 릴리즈 수행
	DebugAssertMsg(m_pMapLayer, "[SGActorBox] 맵 레이어가 존재하지 않습니다.");

	// Step 1. 맵에 로딩되지 않은 풀의 엑터들을 정리해줘야한다.
	// Step 2. 맵에 로딩된 풀에 없는 엑터들을 정리해줘야한다.

	//cleanUpList<SGCharacter>(m_vCharacters);
	_LogDebug_("Step Before. 정리 대기중 엑터 수: %d개", m_hRemoveActorMap.Size());
	updateCleanUp();
	DebugAssertMsg(m_hRemoveActorMap.Size() == 0, "아직 반환안된 액터가 있습니다. 이러면 안됩니다.");

	int iStep1ReleaseCount = 0;
	int iStep2ReleaseCount = 0;

	// Step1. 현재 맵에 로딩된 액터들 릴리즈
	iStep1ReleaseCount += releaseList<Character>(m_vCharacters);
	iStep1ReleaseCount += releaseList<Projectile>(m_vProjectiles);
	iStep1ReleaseCount += releaseList<Monster>(m_vMonsters);
	iStep1ReleaseCount += releaseList<MapObject>(m_vMapObjects);
	iStep1ReleaseCount += releaseList<Effect>(m_vEffectList);
	_LogDebug_("정리된 로디드 액터 수: %d개", iStep1ReleaseCount);
	DebugAssertMsg(iStep1ReleaseCount == m_hActorMap.Size(), "Step1. 실패");


	// Step2. 현재 맵에 로딩되지 않은 엑터들 릴리즈
	iStep2ReleaseCount += releasePool<Effect>(m_hEffectPool);
	iStep2ReleaseCount += releasePool<Monster>(m_hMonsterPool);
	iStep2ReleaseCount += releasePool<Projectile>(m_hProjectilePool);
	iStep2ReleaseCount += releasePool<MapObject>(m_hMapObjectPool);
	_LogDebug_("정리된 언로디드 액터 수: %d개", iStep2ReleaseCount);

	m_vZOrderedActors.Clear();
	m_vCollidableMapObjects.Clear();
	m_vPhysicsActors.Clear();
	m_hActorMap.Clear();

	auto pMap = m_pMapLayer;
	CC_SAFE_RELEASE_NULL(m_pMapLayer);
	m_bCleared = true;
}


void ActorBox::clearRoom() {
	// 여기선 해줘야함 삭제시키는게 목적이 아니라 풀로 복귀 시키는게 목적이기 때문
	int iCleanUpCount = 0;

	iCleanUpCount += cleanUpList<Projectile>(m_vProjectiles);
	iCleanUpCount += cleanUpList<Monster>(m_vMonsters);
	iCleanUpCount += cleanUpList<MapObject>(m_vMapObjects);
	iCleanUpCount += cleanUpList<Effect>(m_vEffectList);
	iCleanUpCount += cleanUpList<Character>(m_vCharacters);

	_LogDebug_("Step Before. 정리 대기중 엑터 수: %d개", m_hRemoveActorMap.Size());
	if (updateCleanUp() != iCleanUpCount) {
		DebugAssertMsg(false, "정리가 제대로 수행되지 않았습니다.");
	}
	DebugAssertMsg(m_hRemoveActorMap.Size() == 0, "아직 반환안된 액터가 있습니다. 이러면 안됩니다.");

	m_vZOrderedActors.Clear();
	m_vCollidableMapObjects.Clear();
	m_vPhysicsActors.Clear();
	m_hActorMap.Clear();

	// 풀 4개는 건들지 않음

	CC_SAFE_RELEASE_NULL(m_pMapLayer);
	m_bCleared = true;
}

void ActorBox::update(float dt) {

	if (m_bCleared)
		return;

	DebugAssertMsg(m_pMapLayer, "[SGActorBox] 맵 레이어가 세팅되지 않았습니다.");

	updateZOrder();
	updateActors(dt);
	updateCleanUp();
}

int ActorBox::updateCleanUp() {
	int iCleanUpCount = m_qRemovedActors.Size();
	while (!m_qRemovedActors.IsEmpty()) {
		Actor* pRemovedActor = m_qRemovedActors.Front();
		m_qRemovedActors.Dequeue();
		m_hRemoveActorMap.Remove(pRemovedActor);
		cleanUp(pRemovedActor);
		m_pMapLayer->removeChild(pRemovedActor);
	}

	DebugAssertMsg(m_hRemoveActorMap.IsEmpty(), "아직 삭제되지 않은 액터가 있습니다. 말도 안됩니다.");
	return iCleanUpCount;
}


void ActorBox::updateZOrder() {
	// 틱 이전, 이후 비교했을때 높은 확률로 대부분 정렬되어 있으므로 삽입 정렬 진행
	sortZOrderActor();

	for (int iOrder = 1; iOrder <= m_vZOrderedActors.Size(); ++iOrder) {
		m_pMapLayer->reorderChild(m_vZOrderedActors[iOrder - 1], iOrder);
	}
}

void ActorBox::updateActors(float dt) {
	for (int i = 0; i < m_vProjectiles.Size(); ++i) {
		m_vProjectiles[i]->update(dt);
	}

	for (int i = 0; i < m_vMonsters.Size(); ++i) {
		m_vMonsters[i]->update(dt);
	}

	for (int i = 0; i < m_vMapObjects.Size(); ++i) {
		m_vMapObjects[i]->update(dt);
	}

	for (int i = 0; i < m_vCharacters.Size(); ++i) {
		m_vCharacters[i]->update(dt);
	}

	for (int i = 0; i < m_vEffectList.Size(); ++i) {
		m_vEffectList[i]->update(dt);
	}
}


Character* ActorBox::createCharacterOnMap(CharType_t charType, float x, float y, VisualInfo& info) {
	DebugAssertMsg(m_pMapLayer, "맵 레이어 생성 및 init 후 캐릭터를 생성해주세요");

	Character* pCharacter = Character::create(charType, info);

	pCharacter->setPositionRealCenter(x, y);
	pCharacter->retain();
	pCharacter->setAllyFlag(0);
	pCharacter->setMapLayer(m_pMapLayer);

	registerCharacter(pCharacter);
	m_pMapLayer->addChild(pCharacter);
	return pCharacter;
}

Projectile* ActorBox::createProejctileOnMap(Actor* spawner, int projectileId) {
	DebugAssertMsg(m_pMapLayer, "맵 레이어 생성 및 init 후 프로젝틸을 생성해주세요");

	ProjectileInfo* pInfo = CoreDataManager_v->getProjectileInfo(spawner->getType(), projectileId);
	

	if (!m_hProjectilePool.Exist(projectileId)) {
		m_hProjectilePool.Insert(Move(projectileId), SGLinkedList<Projectile*>());
	}
	
	Projectile* pProjectile = nullptr;
	SGLinkedList<Projectile*>& projectileList = m_hProjectilePool[projectileId];

	if (projectileList.IsEmpty()) {
		ActorListener* pListener = CoreActorListenerManager_v->createProjectileListener(pInfo->ProjectileListenerCode);

		pProjectile = Projectile::create(spawner, pInfo);
		pProjectile->getListenerCollection().add(pListener);
		pProjectile->retain();

		pListener->injectActor(pProjectile);
		pListener->onCreated();

	} else {
		pProjectile = projectileList.Front();
		pProjectile->initVariables();
		pProjectile->setSpawner(spawner);											// 위치 세팅전 스포너 세팅 먼저 해줘야함
		pProjectile->initThicknessBox(pProjectile->getBaseInfo()->ThicknessBox);	// 두께 박스 위치 재조정
		pProjectile->initPosition();												// 살제 위치 초기화
		pProjectile->getListenerCollection().onCreated();
		projectileList.PopFront();
	}


	pProjectile->runAnimation(1);
	pProjectile->setAllyFlag(spawner->getAllyFlag());
	pProjectile->setMapLayer(m_pMapLayer);

	if (pInfo->SpawnEffect->Code != InvalidValue_v) {
		Effect* pSpawnEffect = createEffectOnMapBySpawner(spawner, pInfo->SpawnEffect->Code, pInfo->SpawnEffectOffsetX, pInfo->SpawnEffectOffsetY);
		pSpawnEffect->setLocalZOrder(spawner->getLocalZOrder() + 1);
	}
	
	registerProjectile(pProjectile);
	m_pMapLayer->addChild(pProjectile);
	return pProjectile;
}


Monster* ActorBox::createMonsterOnMap(int monsterCode, int aiCode, float x, float y) {
	DebugAssertMsg(m_pMapLayer, "맵 레이어 생성 및 init 후 몬스터를 생성해주세요");

	MonsterInfo* pMonsterInfo = CoreDataManager_v->getMonsterInfo(monsterCode);
	AIInfo* pAIInfo = CoreDataManager_v->getAIInfo(aiCode);

	if (!m_hMonsterPool.Exist(monsterCode)) {
		m_hMonsterPool.Insert(Move(monsterCode), SGLinkedList<Monster*>());
	}

	Monster* pMonster = nullptr;
	SGLinkedList<Monster*>& monsterList = m_hMonsterPool[monsterCode];

	if (monsterList.IsEmpty()) {
		pMonster = Monster::create(pMonsterInfo, pAIInfo);
		pMonster->retain();
	} else {
		pMonster = monsterList.Front();
		pMonster->initVariables();
		monsterList.PopFront();
	}

	pMonster->setPositionReal(x, y);
	pMonster->setMapLayer(m_pMapLayer);
	pMonster->setAllyFlag(1);

	registerMonster(pMonster);
	m_pMapLayer->addChild(pMonster);

	return pMonster;
}

MapObject* ActorBox::createMapObjectOnMap(int mapObjectCode, float x, float y) {
	DebugAssertMsg(m_pMapLayer, "맵 레이어 생성 및 init 후 옵스터클을 생성해주세요");

	MapObjectInfo* pMapObjectInfo = CoreDataManager_v->getMapObjectInfo(mapObjectCode);

	if (!m_hMapObjectPool.Exist(mapObjectCode)) {
		m_hMapObjectPool.Insert(Move(mapObjectCode), SGLinkedList<MapObject*>());
	}

	MapObject* pMapObject = nullptr;
	SGLinkedList<MapObject*>& mapObjectList = m_hMapObjectPool[mapObjectCode];

	if (mapObjectList.IsEmpty()) {
		pMapObject = MapObject::create(pMapObjectInfo);
		pMapObject->retain();
	} else {
		pMapObject = mapObjectList.Front();
		pMapObject->initVariables();
		mapObjectList.PopFront();
	}
	pMapObject->setPositionReal(x, y);
	pMapObject->setMapLayer(m_pMapLayer);

	registerMapObject(pMapObject);
	m_pMapLayer->addChild(pMapObject);

	return pMapObject;
}

void ActorBox::registerPlayerOnMap(Player* player) {

	for (int i = 0; i < m_vCharacters.Size(); ++i) {
		if (m_vCharacters[i] == player) {
			DebugAssertMsg(false, "이미 캐릭터가 맵에 포함되어 있습니다.");
			return;
		}
	}

	registerCharacter(player);
	player->retain();
	m_pMapLayer->addChild(player);
}


Effect* ActorBox::createEffectOnMapBySpawner(Actor* spawner, int effectCode, float offsetX, float offsetY) {
	Effect* pEffect = createEffectOnMap(effectCode);

	SGSize spawnerCanvsSize = spawner->getCanvasSize();
	SGVec2 spawnerCanvasPos = spawner->getCanvasPositionReal();

	pEffect->setSpriteDirection(spawner->getSpriteDirection());

	if (spawner->getSpriteDirection() == SpriteDirection::Right) {
		pEffect->setPositionReal(
			spawnerCanvasPos.x + offsetX,
			spawnerCanvasPos.y + offsetY
		);
	} else {
		pEffect->setPositionReal(
			spawnerCanvasPos.x + spawnerCanvsSize.width - offsetX,
			spawnerCanvasPos.y + offsetY
		);
	}

	return pEffect;
}

Effect* ActorBox::createEffectOnMapBySpawner(Actor* spawner, int effectCode, const SGVec2& offset) {
	return createEffectOnMapBySpawner(spawner, effectCode, offset.x, offset.y);
}

Effect* ActorBox::createEffectOnMapAbsolute(int effectCode, SpriteDirection_t direction, float x, float y, int zOrder) {
	Effect* pEffect = createEffectOnMap(effectCode);
	pEffect->setSpriteDirection(direction);
	pEffect->setPositionRealCenter(x, y);
	pEffect->setLocalZOrder(zOrder);
	return pEffect;
}

Effect* ActorBox::createEffectOnMapAbsolute(int effectCode, SpriteDirection_t direction, const SGVec2& pos, int zOrder) {
	return createEffectOnMapAbsolute(effectCode, direction, pos.x, pos.y, zOrder);
}

Effect* ActorBox::createEffectOnMapAbsolute(int effectCode, float x, float y, int zOrder) {
	Effect* pEffect = createEffectOnMap(effectCode);
	pEffect->setPositionRealCenter(x, y);
	pEffect->setLocalZOrder(zOrder);
	return pEffect;
}

Effect* ActorBox::createEffectOnMapAbsolute(int effectCode, const SGVec2& pos, int zOrder) {
	return createEffectOnMapAbsolute(effectCode, pos.x, pos.y, zOrder);
}


Effect* ActorBox::createEffectOnMapTargetCollision(int effectCode, SpriteDirection_t direction, const HitInfo& info, bool randomRotation) {
	Effect* pEffect = createEffectOnMapAbsolute(effectCode, info.HitRect.getMid(), info.HitTarget->getLocalZOrder() + 1);
	pEffect->setSpriteDirection(direction);

	if (randomRotation) {
		pEffect->setRotation(SGRandom::random_real(0.0f, 360.0f));
	}

	return pEffect;
}

Effect* ActorBox::createEffectOnMapTargetCollision(int effectCode, const HitInfo& info, bool randomRotation) {
	return createEffectOnMapTargetCollision(effectCode, SpriteDirection::Right, info, randomRotation);
}

Effect* ActorBox::createEffectOnMapTargetCollision(int effectCode, const HitInfo& info, float offsetX, float offsetY, bool randomRotation) {
	Effect* pEffect = createEffectOnMapTargetCollision(effectCode, info, randomRotation);
	pEffect->setPosition(pEffect->getPositionX() + offsetX, pEffect->getPositionX() + offsetY);
	return pEffect;
}

Effect* ActorBox::createEffectOnMapTargetCollision(int effectCode, const HitInfo& info, const SGVec2& offset, bool randomRotation) {
	return createEffectOnMapTargetCollision(effectCode, info, offset.x, offset.y, randomRotation);
}

Effect* ActorBox::createEffectOnMap(int effectCode) {
	DebugAssertMsg(m_pMapLayer, "맵 레이어 생성 및 init 후 이펙트를 생성해주세요");

	EffectInfo* pEffectInfo = CoreDataManager_v->getEffectInfo(effectCode);

	if (!m_hEffectPool.Exist(effectCode)) {
		m_hEffectPool.Insert(Move(effectCode), SGLinkedList<Effect*>());
	}

	Effect* pEffect = nullptr;
	SGLinkedList<Effect*>& effectList = m_hEffectPool[effectCode];

	if (effectList.IsEmpty()) {
		pEffect = Effect::create(pEffectInfo);
		pEffect->retain();
	}
	else {
		pEffect = effectList.Front();
		pEffect->initVariables();
		effectList.PopFront();
	}

	pEffect->runAnimation(1);
	pEffect->initThicknessBox({ 0, 0, 0, 0 });
	pEffect->setMapLayer(m_pMapLayer);
	registerEffect(pEffect);
	m_pMapLayer->addChild(pEffect);
	return pEffect;
}


void ActorBox::registerCharacter(Character* character) {
	m_vCharacters.PushBack(character);
	m_vPhysicsActors.PushBack(character);
	m_vZOrderedActors.PushBack(character);
	registerActor(character);
}

void ActorBox::registerProjectile(Projectile* projectile) {
	m_vProjectiles.PushBack(projectile);
	m_vZOrderedActors.PushBack(projectile);
	registerActor(projectile);
}

void ActorBox::registerMonster(Monster* monster) {
	m_vMonsters.PushBack(monster);
	m_vZOrderedActors.PushBack(monster);
	m_vPhysicsActors.PushBack(monster);
	registerActor(monster);
}

void ActorBox::registerMapObject(MapObject* mapObject) {
	m_vMapObjects.PushBack(mapObject);

	switch(mapObject->getObjectType()) {
	case MapObjectType::Obstacle: {
		MapObjectObstacleInfo* pInfo = dynamic_cast<MapObjectObstacleInfo*>(mapObject->getBaseInfo());

		if (pInfo->ZOrederable)
			m_vZOrderedActors.PushBack(mapObject);
		if (pInfo->Colliadalble)
			m_vCollidableMapObjects.PushBack(mapObject);

		break;
	}
	case MapObjectType::Gate: {
		m_vZOrderedActors.PushBack(mapObject);
		break;
	}
	}
	

	registerActor(mapObject);
}

void ActorBox::registerEffect(Effect* effect) {
	m_vEffectList.PushBack(effect);
	registerActor(effect);
}


void ActorBox::registerActor(Actor* actor) {
	int iUniqueId = ++m_iIdSequence;
	actor->setActorId(iUniqueId);
	if (m_hActorMap.Insert(iUniqueId, actor) == false) {
		DebugAssertMsg(false, "이미 %s 액터가 ActorMap에 등록되어 있습니다. (코드: %d)", actor->getTypeName(), iUniqueId);
	}
}

void ActorBox::cleanUp(Actor* actor) {
	switch (actor->getType()) {
	case ActorType::Character: cleanUpCharacter(dynamic_cast<Character*>(actor)); break;
	case ActorType::Monster: cleanUpMonster(dynamic_cast<Monster*>(actor)); break;
	// case ActorType::Npc: cleanUpNpc(dynamic_cast<Character*>(actor)); break;
	case ActorType::Projectile: cleanUpProjectile(dynamic_cast<Projectile*>(actor)); break;
	case ActorType::MapObject: cleanUpMapObject(dynamic_cast<MapObject*>(actor)); break;
	case ActorType::Effect: cleanUpEffect(dynamic_cast<Effect*>(actor)); break;
	default: DebugAssert(false);
	}

	actor->getListenerCollection().onCleanUp();
}

void ActorBox::sortZOrderActor() {
	m_vZOrderedActors.SortInsertion([](Actor* lhs, Actor* rhs) {
		return lhs->getThicknessBoxRect().getMidY() > rhs->getThicknessBoxRect().getMidY();
	});

	
}

void ActorBox::cleanUpAtNextFrame(Actor* actor) {
	// 이미 존재하는 경우 무시
	if (m_hRemoveActorMap.Exist(actor)) {
		return;
	}
	m_hRemoveActorMap.Insert(actor, actor);
	m_qRemovedActors.Enqueue(actor);
}

void ActorBox::unregisterZOrderActor(Actor* actor) {

	int iOffset = m_vZOrderedActors.OffsetLowerBound(actor->getLocalZOrder(), [](const Actor* actor, int zorder) {
		return actor->getLocalZOrder() < zorder;
	});

	DebugAssertMsg(iOffset != -1, "Z 오더 목록에서 대상을 찾지 못했습니다.");
	DebugAssertMsg(m_vZOrderedActors[iOffset] == actor, "Z 오더 목록에서 찾은 대상이 동일하지 않습니다. 로직 오류! 로직 오류!");

	if (m_vZOrderedActors[iOffset] == actor) {
		m_vZOrderedActors.RemoveAt(iOffset);
	}
}

void ActorBox::unregisterProjectile(Projectile* projectile) {
	if (!m_vProjectiles.Remove(projectile)) {
		DebugAssertMsg(false, "프로젝틸 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void ActorBox::unregisterCharacter(Character* chracter) {
	if (!m_vCharacters.Remove(chracter)) {
		DebugAssertMsg(false, "캐릭터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void ActorBox::unregisterMonster(Monster* mosnter) {
	if (!m_vMonsters.Remove(mosnter)) {
		DebugAssertMsg(false, "몬스터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void ActorBox::unregisterMapObject(MapObject* mapObject) {

	if (!m_vMapObjects.Remove(mapObject)) {
		DebugAssertMsg(false, "옵스터클 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void ActorBox::unregisterColidableMapObject(MapObject* mapObject) {
	if (!m_vCollidableMapObjects.Remove(mapObject)) {
		DebugAssertMsg(false, "충돌 가능한 옵스터클 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}



void ActorBox::unregisterEffect(Effect* effect) {
	if (!m_vEffectList.Remove(effect)) 
		DebugAssertMsg(false, "이펙트 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
}

void ActorBox::unregisterPhysicsActor(Actor* physicsActor) {
	if (!m_vPhysicsActors.Remove(physicsActor))
		DebugAssertMsg(false, "피직스 액터 목록에서 액터를 제거하는데 실패했습니다.");
}

void ActorBox::unregisterActor(Actor* actor) {
	DebugAssertMsg(actor->getActorId() != InvalidValue_v, "올바르지 않은 액터 ID입니다.");
	if (!m_hActorMap.Remove(actor->getActorId()))
		DebugAssertMsg(false, "공통 액터 목록에서 액터를 제거하는데 실패했습니다.");
	else
		actor->setActorId(InvalidValue_v);
}


void ActorBox::cleanUpProjectile(Projectile* projectile) {
	unregisterProjectile(projectile);
	unregisterZOrderActor(projectile);
	unregisterActor(projectile);
	m_hProjectilePool[projectile->getBaseInfo()->Code].PushBack(projectile);
	_LogDebug_("삭제> 플레이어 프로젝틸 (%s), 남은 플레이어 프로젝틸 수 : %d, Z오더 액터 수: %d", projectile->getBaseInfo()->Name.Source(), m_vProjectiles.Size(), m_vZOrderedActors.Size());
}

void ActorBox::cleanUpMonster(Monster* monster) {
	unregisterMonster(monster);
	unregisterZOrderActor(monster);

	if (monster->hasComponent(IComponent::ePhysics))
		unregisterPhysicsActor(monster);

	unregisterActor(monster);
	m_hMonsterPool[monster->getBaseInfo()->Code].PushBack(monster);
	_LogDebug_("삭제> 몬스터 (%s), 남은 몬스터 수 : %d, Z오더 액터 수: %d", monster->getBaseInfo()->Name.Source(), m_vMonsters.Size(), m_vZOrderedActors.Size());
}

void ActorBox::cleanUpMapObject(MapObject* mapObject) {
	unregisterMapObject(mapObject);
	unregisterActor(mapObject);

	MapObjectInfo* pBaseInfo = mapObject->getBaseInfo();

	switch(mapObject->getObjectType()) {
	case MapObjectType::Obstacle: {
		MapObjectObstacleInfo* pInfo = dynamic_cast<MapObjectObstacleInfo*>(mapObject->getBaseInfo());

		if (pInfo->ZOrederable)
			unregisterZOrderActor(mapObject);
		if (pInfo->Colliadalble)
			unregisterColidableMapObject(mapObject);

		break;
	}
	case MapObjectType::Gate: {
		unregisterZOrderActor(mapObject);
		break;
	}
	}

	m_hMapObjectPool[pBaseInfo->Code].PushBack(mapObject);
}

void ActorBox::cleanUpCharacter(Character* character) {
	unregisterCharacter(character);
	unregisterZOrderActor(character);
	unregisterActor(character);
}

void ActorBox::cleanUpEffect(Effect* effect) {
	unregisterEffect(effect);
	unregisterActor(effect);

	m_hEffectPool[effect->getBaseInfo()->Code].PushBack(effect);
}

