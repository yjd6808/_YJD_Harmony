#pragma once

#include <TF/SourceHeader.h>

#include <Common/Enum.h>
#include <JCore/Container/HashMap.h>

/*
 * ��ֹ� Ŭ����
 * ���ο� ���׶� �ö��̴����� �� ä���� �浹üũ�� �� �� �ֵ��� �Ѵ�.
 * ���� Body�� ���� �����ϰ� ������ �� ������ �ڵ尡 �������� �� ����.
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
	float m_fPrecisionCircleDiametor;		// ���е� - 10�̸� ������ 10.0f�� ����� ���θ� �� ä��
	ObstacleShape m_eShape;					// ���
	JCore::Vector<Scale9Sprite*> m_Colliders;		// �׳� Node�� �ص��Ǵµ� ������ ���������� Ȯ���ϸ鼭 �׽�Ʈ�ҷ��� �� Sprite�� ��
};



