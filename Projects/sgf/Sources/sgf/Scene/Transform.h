/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * Transform — GameObject 기본 컴포넌트 (위치/회전/크기 + 행렬 캐시 + dirty 전파)
 *
 * [역할]
 * GameObject마다 1개씩 자동 생성되는 부품. 로컬 SRT(Scale/Rotation/Translation)를
 * 보관하고, 그걸 로컬 행렬 → 월드 행렬로 합성해준다.
 * 월드 행렬은 부모 Transform의 월드 행렬과 곱해서 만든다. (트리 상속)
 *
 * [행렬 캐시 + dirty 전파]
 * 로컬 값이 바뀌면 localDirty_, 부모가 바뀌면 worldDirty_가 세워진다.
 * GetWorldMatrix()는 dirty일 때만 재계산하고, 결과를 캐시해 둔다.
 * (게임 오브젝트 트리의 수백~수천 노드에서 프레임당 불필요한 행렬 재계산 방지)
 *
 * [회전]
 * - 2D: SetLocalRotationRad(_f32) — Z축 회전 (SRT2D 패턴)
 * - 3D: SetLocalRotationEuler(_f32 xDeg, _f32 yDeg, _f32 zDeg) — 각 축 오일러
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Scene/Component.h"

NS_SGF_BEGIN

using namespace jc;

class GameObject;

class Transform : public Component
{
	SGF_COMPONENT_TYPE(ctTransform)

public:
	explicit Transform(GameObject* _pOwner);
	virtual ~Transform() override;

	// === 로컬 값 설정/조회 ===
	void SetLocalPosition(const vec3& _position);
	void SetLocalPosition(const vec2& _position) { SetLocalPosition(vec3(_position.x, _position.y, 0.0f)); }
	const vec3& GetLocalPosition() const { return localPosition_; }
	vec2 GetLocalPosition2D() const { return vec2(localPosition_.x, localPosition_.y); }

	void SetLocalScale(const vec3& _scale);
	void SetLocalScale(_f32 _uniform) { SetLocalScale(vec3(_uniform, _uniform, _uniform)); }
	const vec3& GetLocalScale() const { return localScale_; }

	void SetLocalRotationRad(_f32 _radian);			// Z축 회전 (2D)
	_f32 GetLocalRotationRad() const { return localRotationZ_; }
	void SetLocalRotationEuler(_f32 _xDeg, _f32 _yDeg, _f32 _zDeg);	// 3D 오일러(도)
	const vec3& GetLocalRotationEuler() const { return localRotationEuler_; }

	// === 행렬 ===
	const mat4& GetLocalMatrix();		// 로컬 SRT 합성 (캐시)
	const mat4& GetWorldMatrix();		// 부모 월드 x 로컬 (캐시)
	vec3 GetWorldPosition() const;		// 월드 이동 성분 (트리 상속 위치)
	vec2 GetWorldPosition2D() const;	// 월드 위치 (2D — y 위+ 그대로)

	// === dirty 전파 ===
	void SetDirty();					// 이 노드의 world + 자식 전파

private:
	friend class GameObject;			// GameObject가 부모 Transform을 설정한다
	void SetParent(Transform* _pParent);
	void RebuildLocal();

private:
	GameObject* pOwner_;				// 소유 게임 오브젝트 (소유 아님)
	Transform* pParent_;				// 부모 Transform (트리 상속용, 소유 아님)

	vec3 localPosition_;				// 로컬 위치
	vec3 localScale_ = vec3::One();		// 로컬 크기 (기본 1)
	_f32 localRotationZ_ = 0.0f;		// 2D Z축 회전 (라디안)
	vec3 localRotationEuler_ = vec3::Zero();	// 3D 오일러 (도)

	mat4 local_;						// 로컬 행렬 캐시
	mat4 world_;						// 월드 행렬 캐시
	bool localDirty_ = true;
	bool worldDirty_ = true;
};

NS_SGF_END