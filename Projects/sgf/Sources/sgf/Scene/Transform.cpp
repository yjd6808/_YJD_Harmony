/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * Transform 구현부 — SRT 합성 + dirty 캐시
 */

#include "sgf/Scene/Transform.h"
#include "sgf/Scene/GameObject.h"
#include "jc/Math.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Transform::Transform(GameObject* _pOwner)
	: pOwner_(_pOwner)
	, pParent_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Transform::~Transform()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void Transform::SetLocalPosition(const vec3& _position)
{
	if (localPosition_ == _position) return;
	localPosition_ = _position;
	localDirty_ = true;
	SetDirty();	// 자식 전파
}

//////////////////////////////////////////////////////////////////////////////////////////
void Transform::SetLocalScale(const vec3& _scale)
{
	if (localScale_ == _scale) return;
	localScale_ = _scale;
	localDirty_ = true;
	SetDirty();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Transform::SetLocalRotationRad(_f32 _radian)
{
	if (FloatEqual(localRotationZ_, _radian)) return;
	localRotationZ_ = _radian;
	localDirty_ = true;
	SetDirty();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Transform::SetLocalRotationEuler(_f32 _xDeg, _f32 _yDeg, _f32 _zDeg)
{
	const vec3 euler(_xDeg, _yDeg, _zDeg);
	if (localRotationEuler_ == euler) return;
	localRotationEuler_ = euler;
	localDirty_ = true;
	SetDirty();
}

//////////////////////////////////////////////////////////////////////////////////////////
const mat4& Transform::GetLocalMatrix()
{
	if (localDirty_)
	{
		RebuildLocal();
		localDirty_ = false;
	}
	return local_;
}

//////////////////////////////////////////////////////////////////////////////////////////
const mat4& Transform::GetWorldMatrix()
{
	if (worldDirty_)
	{
		const mat4& local = GetLocalMatrix();
		if (pParent_ != nullptr)
		{
			world_ = pParent_->GetWorldMatrix() * local;
		}
		else
		{
			world_ = local;
		}
		worldDirty_ = false;
	}
	return world_;
}

//////////////////////////////////////////////////////////////////////////////////////////
vec3 Transform::GetWorldPosition() const
{
	// dirty 캐시를 깨지 않기 위해 행렬 복사본으로 추출한다. (일반적: const 메서드에서 재계산 가능)
	Transform* pThis = const_cast<Transform*>(this);
	const mat4& world = pThis->GetWorldMatrix();
	return vec3(world.m[3][0], world.m[3][1], world.m[3][2]);
}

//////////////////////////////////////////////////////////////////////////////////////////
vec2 Transform::GetWorldPosition2D() const
{
	Transform* pThis = const_cast<Transform*>(this);
	const mat4& world = pThis->GetWorldMatrix();
	return vec2(world.m[3][0], world.m[3][1]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Transform::SetDirty()
{
	// 자식의 world가 이 노드에 의존하므로, 자식 트리 전체에 worldDirty_를 전파한다.
	// (실제 자식 목록은 GameObject 트리가 소유 — 이곳에서는 pOwner_를 통해 접근)
	if (pOwner_ == nullptr) return;

	worldDirty_ = true;

	// GameObject에게 자식 Transform들 dirty 전파를 요청한다.
	pOwner_->PropagateTransformDirty();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Transform::SetParent(Transform* _pParent)
{
	pParent_ = _pParent;
	worldDirty_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Transform::RebuildLocal()
{
	// SRT 합성: Scale → Rotation → Translation (행벡터 규약: v * (S * R * T))
	// 2D 모드(Z회전)와 3D 모드(오일러) 중 사용된 쪽 기준으로 합성한다.
	if (FloatEqual(localRotationEuler_.x, 0.0f) && FloatEqual(localRotationEuler_.y, 0.0f)
		&& FloatEqual(localRotationEuler_.z, 0.0f))
	{
		// 2D: Z축 회전만 (일반적인 2D 게임 오브젝트)
		local_ = mat4::Scale(localScale_.x, localScale_.y, localScale_.z)
			* mat4::RotationZ(localRotationZ_)
			* mat4::Translation(localPosition_);
	}
	else
	{
		// 3D: 오일러 각 (도 → 라디안). ZXY 순서 합성.
		const _f32 rad = 3.14159265358979f / 180.0f;
		local_ = mat4::Scale(localScale_.x, localScale_.y, localScale_.z)
			* mat4::RotationZ(localRotationEuler_.z * rad)
			* mat4::RotationX(localRotationEuler_.x * rad)
			* mat4::RotationY(localRotationEuler_.y * rad)
			* mat4::Translation(localPosition_);
	}
}

NS_SGF_END