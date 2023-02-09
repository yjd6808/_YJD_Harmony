#include <TF/Object/Obstacle.h>
#include <TF/Util/NodeUtil.h>

#include <JCore/Container/Vector.h>

bool Obstacle::init2(float width, float height) {
	

	int iColliderCount = 0;			// 총 콜라이더 수
	int iHorizontalCount = 0;		// 가로 1줄을 채우는데 필요한 콜라이더 수
	int iVerticalCount = 0;			// 세로 1줄을 채우는데 필요한 콜라이더 수

	this->setContentSize({ width, height });

	switch (m_eShape) {
	case ObstacleShape::Circle:
		iColliderCount = 1;
		break;
	case ObstacleShape::Rhombus:
		// 마름모 꼴은 크기를 루트2로 나눠서 대각선이 지름이 되도록 작게 만들어주자. break 넣으면 안됨
		this->setContentSize({ this->getContentSize() / sqrt(2) });		
	case ObstacleShape::Rectangle:
		iHorizontalCount = round(this->getContentSize().width / m_fPrecisionCircleDiametor) ;
		iVerticalCount = round(this->getContentSize().height / m_fPrecisionCircleDiametor);

		iColliderCount = iHorizontalCount * iVerticalCount;
		break;
	}

	if (m_eShape == ObstacleShape::Rectangle || m_eShape == ObstacleShape::Rhombus) {
		if (iHorizontalCount == 0)
			iHorizontalCount = 1;

		if (iVerticalCount == 0)
			iVerticalCount = 1;

		// 가로 세로 한줄씩 딱 맞춰주기 위해 넣어줌
		m_Colliders.Resize(iColliderCount + iVerticalCount + iHorizontalCount);

		Vec2 stdPos = this->getPosition();									// 시작 꼭지점 위치, 콜라이더를 채우기 시작할 기준 위치
		Vec2 stdReversePos = this->getPosition() + this->getContentSize();  // 반대 꼭지점 위치
		float fRadious = m_fPrecisionCircleDiametor / 2.0f;

		for (int i = 0; i < iVerticalCount; i++) {
			for (int j = 0; j < iHorizontalCount; j++) {
				Scale9Sprite* pCollider = Scale9Sprite::create(CIRCLE_IMG_FILENAME);
				Vec2 pos;
				pCollider->setContentSize({ m_fPrecisionCircleDiametor, m_fPrecisionCircleDiametor });

				if (i == iVerticalCount - 1) {
					pos.y = stdReversePos.y - fRadious;
				} else {
					pos.y = stdPos.y + (m_fPrecisionCircleDiametor * i) + fRadious;
				}


				if (j == iHorizontalCount - 1) {
					pos.x = stdReversePos.x - fRadious;
				} else {
					pos.x = stdPos.x + (m_fPrecisionCircleDiametor * j) + fRadious;
				}

				pCollider->setPosition(pos);
				pCollider->setVisible(false);

				this->addChild(pCollider);
				m_Colliders.PushBack(pCollider);
			}
		}

		if (m_eShape == ObstacleShape::Rhombus) {
			this->setRotation(45.0f);
		}
	} else if (m_eShape == ObstacleShape::Circle) {
		Scale9Sprite* pCollider = Scale9Sprite::create(CIRCLE_IMG_FILENAME);
		pCollider->setContentSize(this->getContentSize());
		pCollider->setPosition({ this->getContentSize().width / 2, this->getContentSize().height / 2 });
		pCollider->setVisible(false);
		this->addChild(pCollider);
		m_Colliders.PushBack(pCollider);
	}

	return true;
}

Obstacle* Obstacle::create(ObstacleShape shape, float width, float height, float precisionCircleDiametor) {
	Obstacle* sprite = new Obstacle(shape, precisionCircleDiametor);

	if (sprite->init() && shape == ObstacleShape::Circle ? sprite->initWithFile(CIRCLE_IMG_FILENAME) : sprite->initWithFile(RECT_IMG_FILENAME) && sprite->init2(width, height)) {
		sprite->autorelease();
		return sprite;
	}

	CC_SAFE_DELETE(sprite);
	return nullptr;
}

bool Obstacle::IsCollide(Tank* tank) {
	const auto tankColliders = tank->GetColliders();

	// 내부 컬라이더들이 모두 
	for (int i = 0; i < tankColliders.Size(); i++) {
		const Sprite* tankCollider = tankColliders[i];
		const Vec2 tankColliderWorldPos = tank->convertToWorldSpace(tankCollider->getPosition());
		for (int j = 0; j < m_Colliders.Size(); j++) {
			const Sprite* obstacleCollider = m_Colliders[j];
			const Vec2 obstacleColliderWorldPos = this->convertToWorldSpace(obstacleCollider->getPosition());

			if (NodeUtil::IsCollideCircle(tankColliderWorldPos, TANK_WIDTH / 2.0f, obstacleColliderWorldPos, m_fPrecisionCircleDiametor / 2.0f)) {
				return true;
			}
		}
	}

	return false;
}

