/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 6:10:07 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGActorBox.h"

#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGMapLayer.h>

USING_NS_CC;
USING_NS_JC;

SGActorBox::SGActorBox()
	: m_iIdSequence(0)
	, m_pMapLayer(nullptr)
	, m_fZReorderTime(0.0f)
{}

SGActorBox::~SGActorBox() {
	clear();
}


void SGActorBox::init(SGMapLayer* mapLayer) {
	m_iIdSequence = 0;
	DebugAssertMessage(m_pMapLayer == nullptr, "맵 레이어가 이미 할당되어 있습니다. 먼저 clear를 호출해서 비워주세요");

	m_pMapLayer = mapLayer;
	m_pMapLayer->retain();
}

void SGActorBox::clear() {
	if (m_bCleared)
		return;

	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	m_hActorMap.Values().Extension().ForEach([this](SGActor* actor) {
		CC_SAFE_RELEASE(actor);
	});

	CC_SAFE_RELEASE_NULL(m_pMapLayer);
	m_bCleared = true;
}

void SGActorBox::update(float dt) {
	updateZOrder(dt);
	updateActors(dt);
	updateCleanUp();
}

void SGActorBox::updateCleanUp() {
	while (!m_qRemovedActors.IsEmpty()) {
		SGActor* pRemovedActor = m_qRemovedActors.Front();
		m_qRemovedActors.Dequeue();
		m_hRemoveActorMap.Remove(pRemovedActor);

		switch (pRemovedActor->getType()) {
		case ActorType::Projectile: {
			cleanUpProjectile((SGProjectile*)pRemovedActor);
			break;
		}
		case ActorType::Monster: {
			cleanUpMonster((SGMonster*)pRemovedActor);
			break;
		}
		case ActorType::Obstacle: {
			cleanUpObstacle((SGObstacle*)pRemovedActor);
			break;
		}
		case ActorType::Character: {
			cleanUpCharacter((SGCharacter*)pRemovedActor);
			break;
		}
		default: DebugAssert(false);
			m_pMapLayer->removeChild(pRemovedActor);
		}
	}
}

void SGActorBox::updateZOrder(float dt) {

	m_fZReorderTime += dt;

	// 0.1초 정도마다 재정렬 진행해주자.
	if (m_fZReorderTime < ZReodrerDelay_v) {
		return;
	}
	m_fZReorderTime = 0.0f;

	// 틱 이전, 이후 비교했을때 높은 확률로 대부분 정렬되어 있으므로 삽입 정렬 진행
	m_vZOrderedActors.SortInsertion([](SGActor* lhs, SGActor* rhs) {
		return lhs->getThicknessBoxRect().getMidY() > rhs->getThicknessBoxRect().getMidY();
	});

	for (int iOrder = 1; iOrder <= m_vZOrderedActors.Size(); ++iOrder) {
		m_pMapLayer->reorderChild(m_vZOrderedActors[iOrder - 1], iOrder);
	}
}

void SGActorBox::updateActors(float dt) {
	for (int i = 0; i < m_vZOrderedActors.Size(); ++i) {
		m_vZOrderedActors[i]->update(dt);
	}
}


SGCharacter* SGActorBox::createCharacterOnMap(CharType_t charType, float x, float y, SGCharacterInfo& info) {
	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	SGCharacter* pCharacter = SGCharacter::create(charType, info);
	pCharacter->setPositionRealCenter(x, y);
	pCharacter->setAllyFlag(0);
	pCharacter->setMapLayer(m_pMapLayer);
	// pCharacter->retain(); 캐릭터 풀 필요
	registerZOrderActor(pCharacter);
	registerCharacter(pCharacter);
	registerPhysicsActor(pCharacter);
	registerActor(pCharacter);
	m_pMapLayer->addChild(pCharacter);
	return pCharacter;
}

SGProjectile* SGActorBox::createProejctileOnMap(SGActor* spawner, int projectileId) {
	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	SGProjectileInfo* pInfo = SGDataManager::getInstance()->getProjectileInfo(projectileId);

	if (!m_hProjectilePool.Exist(projectileId)) {
		m_hProjectilePool.Insert(Move(projectileId), SGList<SGProjectile*>());
	}

	SGProjectile* pProjectile = nullptr;
	SGList<SGProjectile*>& projectileList = m_hProjectilePool[projectileId];

	if (projectileList.IsEmpty()) {
		pProjectile = SGProjectile::create(spawner, pInfo);
		pProjectile->retain();
	} else {
		pProjectile = projectileList.Front();
		pProjectile->init();
		projectileList.PopFront();
	}

	pProjectile->setAllyFlag(spawner->getAllyFlag());
	pProjectile->setMapLayer(m_pMapLayer);
	registerZOrderActor(pProjectile);
	registerProjectile(pProjectile);
	registerActor(pProjectile);
	m_pMapLayer->addChild(pProjectile);
	return pProjectile;
}


SGMonster* SGActorBox::createMonsterOnMap(int monsterCode, int aiCode, float x, float y) {
	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	SGDataManager* pDataManager = SGDataManager::getInstance();

	SGMonsterInfo* pMonsterInfo = pDataManager->getMonsterInfo(monsterCode);
	SGAIInfo* pAIInfo = pDataManager->getAIInfo(aiCode);

	if (!m_hMonsterPool.Exist(monsterCode)) {
		m_hMonsterPool.Insert(Move(monsterCode), SGList<SGMonster*>());
	}

	SGMonster* pMonster = nullptr;
	SGList<SGMonster*>& monsterList = m_hMonsterPool[monsterCode];

	if (monsterList.IsEmpty()) {
		pMonster = SGMonster::create(pMonsterInfo, pAIInfo);
		pMonster->retain();
	} else {
		pMonster = monsterList.Front();
		pMonster->init();
		monsterList.PopFront();
	}

	pMonster->setPositionReal(x, y);
	pMonster->setMapLayer(m_pMapLayer);
	pMonster->setAllyFlag(1);

	registerZOrderActor(pMonster);
	registerMonster(pMonster);
	registerPhysicsActor(pMonster);
	registerActor(pMonster);
	m_pMapLayer->addChild(pMonster);

	return pMonster;
}

SGObstacle* SGActorBox::createObstacleOnMap(int obstacleCode, float x, float y) {
	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	SGObstacleInfo* pObstacleInfo = SGDataManager::getInstance()->getObstacleInfo(obstacleCode);

	if (!m_hObstaclePool.Exist(obstacleCode)) {
		m_hObstaclePool.Insert(Move(obstacleCode), SGList<SGObstacle*>());
	}

	SGObstacle* pObstacle = nullptr;
	SGList<SGObstacle*>& obstacleList = m_hObstaclePool[obstacleCode];

	if (obstacleList.IsEmpty()) {
		pObstacle = SGObstacle::create(pObstacleInfo);
		pObstacle->retain();
	} else {
		pObstacle = obstacleList.Front();
		pObstacle->init();
		obstacleList.PopFront();
	}
	pObstacle->setPositionReal(x, y);
	pObstacle->setMapLayer(m_pMapLayer);

	if (pObstacleInfo->ZOrederable)
		registerZOrderActor(pObstacle);

	if (pObstacleInfo->Colliadalble)
		registerObstacle(pObstacle);

	registerActor(pObstacle);
	m_pMapLayer->addChild(pObstacle);

	return pObstacle;
}




void SGActorBox::registerZOrderActor(SGActor* actor) {
	m_vZOrderedActors.PushBack(actor);
}

void SGActorBox::registerCharacter(SGCharacter* character) {
	m_vCharacters.PushBack(character);
}

void SGActorBox::registerProjectile(SGProjectile* projectile) {
	m_vProjectiles.PushBack(projectile);
}

void SGActorBox::registerMonster(SGMonster* monster) {
	m_vMonsters.PushBack(monster);
}

void SGActorBox::registerObstacle(SGObstacle* obstacle) {
	m_vCollidableObstacles.PushBack(obstacle);
}

void SGActorBox::registerPhysicsActor(SGPhysicsActor* physicsActor) {
	m_vPhysicsActors.PushBack(physicsActor);
}

void SGActorBox::registerActor(SGActor* actor) {
	int iUniqueId = ++m_iIdSequence;
	actor->setActorId(iUniqueId);
	m_hActorMap.Insert(iUniqueId, actor);
}

void SGActorBox::registerCleanUp(SGActor* actor) {
	// 이미 존재하는 경우 무시
	if (m_hRemoveActorMap.Exist(actor)) {
		return;
	}
	m_hRemoveActorMap.Insert(actor, actor);
	m_qRemovedActors.Enqueue(actor);
}

void SGActorBox::unregisterZOrderActor(SGActor* actor) {
	int iOffset = m_vZOrderedActors.OffsetLowerBound(actor->getLocalZOrder(), [](const SGActor* actor, int zorder) {
		return actor->getLocalZOrder() < zorder;
	});

	DebugAssertMessage(iOffset != -1, "Z 오더 목록에서 대상을 찾지 못했습니다.");

	if (m_vZOrderedActors[iOffset] == actor) {
		m_vZOrderedActors.RemoveAt(iOffset);
	}
}

void SGActorBox::unregisterProjectile(SGProjectile* projectile) {
	if (m_vProjectiles.Remove(projectile) == false) {
		DebugAssertMessage(false, "프로젝틸 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterCharacter(SGCharacter* chracter) {
	if (m_vCharacters.Remove(chracter)) {
		DebugAssertMessage(false, "캐릭터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterMonster(SGMonster* mosnter) {
	if (m_vMonsters.Remove(mosnter) == false) {
		DebugAssertMessage(false, "몬스터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterObstacle(SGObstacle* obstacle) {
	if (obstacle->getBaseInfo()->Colliadalble)
		if (m_vCollidableObstacles.Remove(obstacle))
			DebugAssertMessage(false, "옵스터클 목록에서 삭제하고자하는 대상을 찾지못했습니다.");


}

void SGActorBox::unregisterPhysicsActor(SGPhysicsActor* physicsActor) {
	if (m_vPhysicsActors.Remove(physicsActor))
		DebugAssertMessage(false, "피직스 액터 목록에서 액터를 제거하는데 실패했습니다.");
}

void SGActorBox::unregisterActor(SGActor* actor) {
	DebugAssertMessage(actor->getActorId() != InvalidValue_v, "올바르지 않은 액터 ID입니다.");
	if (m_hActorMap.Remove(actor->getActorId()))
		DebugAssertMessage(false, "공통 액터 목록에서 액터를 제거하는데 실패했습니다.");
}


void SGActorBox::cleanUpProjectile(SGProjectile* projectile) {
	unregisterProjectile(projectile);
	unregisterZOrderActor(projectile);

	Log("삭제> 플레이어 프로젝틸 (%s), 남은 플레이어 프로젝틸 수 : %d, Z오더 액터 수: %d\n", projectile->getBaseInfo()->Name.Source(), m_vProjectiles.Size(), m_vZOrderedActors.Size());
}

void SGActorBox::cleanUpMonster(SGMonster* monster) {
	unregisterMonster(monster);
	unregisterZOrderActor(monster);
	unregisterPhysicsActor(monster);

	Log("삭제> 몬스터 (%s), 남은 몬스터 수 : %d, Z오더 액터 수: %d\n", monster->getBaseInfo()->Name.Source(), m_vMonsters.Size(), m_vZOrderedActors.Size());
}

void SGActorBox::cleanUpObstacle(SGObstacle* obstacle) {
	unregisterObstacle(obstacle);

	if (obstacle->getBaseInfo()->ZOrederable)
		unregisterZOrderActor(obstacle);
}

void SGActorBox::cleanUpCharacter(SGCharacter* character) {
	unregisterCharacter(character);
	unregisterZOrderActor(character);
	unregisterActor(character);
}


