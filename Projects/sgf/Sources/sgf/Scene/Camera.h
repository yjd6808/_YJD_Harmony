/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:30:00 AM
 * 수정일: 8/16/2026 (GameObject 파생 — Camera는 컴포넌트가 아니라 씬 요소다)
 * =====================
 * 카메라 (추상 베이스 — GameObject 파생)
 *
 * [왜 GameObject 파생인가? (핵심)]
 * 카메라는 "부착되는 부품(컴포넌트)"이 아니라 그 자체로 씬 요소다.
 * - 위치/회전을 갖는다 (GameObject 기본 Transform 자동 보유)
 * - 트리에 배치할 수 있다 (AddChild 가능)
 * - 고유 ID(GID)/DataMap을 자동으로 갖는다
 * - SelectCamera(GameObject*)가 타입 캐스팅 없이 자연스럽게 성립한다
 *
 * [계약]
 * View() / Projection()은 Camera2D/3D 파생이 구현한다.
 * enabled_ = false면 렌더에서 제외한다. (미래: 다중 카메라 분할 화면)
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Scene/GameObject.h"

NS_SGF_BEGIN

using namespace jc;

class Camera : public GameObject
{
public:
	using GameObject::GameObject;

	virtual ~Camera() override = default;

	// 월드 → 카메라 기준 행렬 (파생의 Rebuild2D/Rebuild3D가 view_를 갱신한다)
	mat4 View() const { return view_; }

	// 카메라 기준 → NDC 행렬 (파생의 Rebuild2D/Rebuild3D가 projection_을 갱신한다)
	mat4 Projection() const { return projection_; }

	// 뷰 x 투영 합성 (셰이더에 올리는 값) — 파라미터 변경 시에만 재계산한다.
	mat4 ViewProjection() const
	{
		if (vpDirty_)
		{
			vp_ = view_ * projection_;
			vpDirty_ = false;
		}
		return vp_;
	}

	// 카메라 파라미터가 바뀌면 파생 클래스(Rebuild2D/Rebuild3D)가 호출한다.
	void InvalidateViewProjection() const { vpDirty_ = true; }

	bool enabled_ = true;		// false면 렌더 제외 (미래: 다중 카메라 분할 화면)
	mat4 view_ = mat4::Identity();			// 뷰 행렬 (파생의 Rebuild2D/Rebuild3D가 갱신)
	mat4 projection_ = mat4::Identity();	// 투영 행렬 (파생의 Rebuild2D/Rebuild3D가 갱신)
	mutable mat4 vp_ = mat4::Identity();	// 뷰 x 투영 캐시
	mutable bool vpDirty_ = true;			// 캐시 무효 여부
};

NS_SGF_END