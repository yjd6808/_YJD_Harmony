#pragma once

#include <TF/SourceHeader.h>

#include <Common/Enum.h>
#include <JCore/Container/HashMap.h>

/*
 * 장애물 클래스
 * 내부에 동그란 컬라이더들을 꽉 채워서 충돌체크를 할 수 있도록 한다.
 * 물리 Body를 쓰면 간단하게 가능할 것 같은데 코드가 복잡해질 것 같다.
 */

class Tank;
class Obstacle : public Scale9Sprite
{
public:
	Obstacle(ObstacleShape shape, float precisionCircleDiametor) : m_eShape(shape), m_fPrecisionCircleDiametor(precisionCircleDiametor) {}
	bool init2(float width, float height);
	static Obstacle* create(ObstacleShape shape, float width, float height, float precisionCircleDiametor = 25.0f);

	bool IsCollide(Tank* node);
private:
	float m_fPrecisionCircleDiametor;		// 정밀도 - 10이면 지름이 10.0f인 원들로 내부를 꽉 채움
	ObstacleShape m_eShape;					// 모양
	JCore::Vector<Scale9Sprite*> m_Colliders;		// 그냥 Node로 해도되는데 눈으로 직접적으로 확인하면서 테스트할려고 걍 Sprite를 씀
};



