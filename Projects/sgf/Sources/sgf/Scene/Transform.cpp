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

// dirty stamp 전역 시계 — Transform::SetDirty가 매 변경마다 증가시킨다. (B-5: lazy 감지용)
namespace
{
	_u64 g_transformStamp = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
Transform::Transform(GameObject* _pOwner)
	: pParent_(nullptr)
	, dirtyStamp_(++g_transformStamp)	// 최초 1회는 반드시 계산되도록 변경 stamp를 부여한다.
{
	// _pOwner: 소유 게임 오브젝트 — 현재는 파생/조회에서 필요하지 않다. (시그니처는 GameObject 호출 규약 유지)
	(void)_pOwner;
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
	SetDirty();	// dirty 표시 (lazy — 자식 전파 없음)
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
	// 부모 체인에서 가장 최근의 변경 stamp를 찾는다. (O(깊이) — 트리 깊이는 얕다)
	// 부모가 이미 재계산했더라도 stamp는 유지되므로, 자식이 나중에 조회해도 감지할 수 있다. (B-5)
	_u64 newest = dirtyStamp_;
	for (const Transform* p = pParent_; p != nullptr; p = p->pParent_)
	{
		if (p->dirtyStamp_ > newest)
		{
			newest = p->dirtyStamp_;
		}
	}

	// 마지막 계산 시각보다 새 변경이 있으면 재계산한다.
	if (newest > computedStamp_)
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
		computedStamp_ = newest;
	}
	return world_;
}

//////////////////////////////////////////////////////////////////////////////////////////
vec3 Transform::GetWorldPosition()
{
	// GetWorldMatrix()가 lazy 캐시를 갱신하므로 non-const 조회다. (A-9)
	const mat4& world = GetWorldMatrix();
	return vec3(world.m[3][0], world.m[3][1], world.m[3][2]);
}

//////////////////////////////////////////////////////////////////////////////////////////
vec2 Transform::GetWorldPosition2D()
{
	const mat4& world = GetWorldMatrix();
	return vec2(world.m[3][0], world.m[3][1]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Transform::SetDirty()
{
	// lazy: 자식 전파 없이 자기 stamp만 갱신한다.
	// 자식/손자는 GetWorldMatrix() 조회 시 부모 체인에서 이 stamp를 발견해 재계산한다. (B-5)
	dirtyStamp_ = ++g_transformStamp;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Transform::SetParent(Transform* _pParent)
{
	pParent_ = _pParent;
	dirtyStamp_ = ++g_transformStamp;	// 부모가 바뀌었으니 stamp 갱신 (lazy — 자식 전파 없음)
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