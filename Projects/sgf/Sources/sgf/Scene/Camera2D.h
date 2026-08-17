/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (Camera를 GameObject 파생으로 분리 — 2D 전용)
 * =====================
 * Camera2D — 직교 카메라 (GameObject 파생)
 *
 * [역할]
 * 화면 왼쪽 아래 (0,0), 오른쪽 위 (width,height)인 2D 좌표계를 만든다.
 * - SetOrthographic2D: 카메라 구성 (렌더 전 1회)
 * - SetPosition2D/SetZoom: 이동/확대
 * - ScreenToWorld2D/WorldToScreen2D: 픽/UI 좌표 변환
 * - DriveDefault2D: 방향키 이동 + 휠 줌 (매 프레임 1줄)
 *
 * [변경]
 * Camera(GameObject 파생)를 상속 → Transform/GID/DataMap 자동 보유, 트리 배치 가능.
 * Scene::CreateDefaultCamera(Camera2D::CreateDefault())로 생성·소유된다.
 */

#pragma once

#include "jc/Math.h"
#include "jc/Time.h"
#include "sgf/Scene/Camera.h"

NS_SGF_BEGIN

using namespace jc;

class InputManager;

class Camera2D : public Camera
{
public:
	Camera2D();

	// [팩토리] 기본 2D 카메라 생성 — 이름 "DefaultCamera2D"의 GameObject (자체가 카메라)
	static Camera2D* CreateDefault();

	// 2D 직교 카메라로 설정한다. (렌더 전 1회 — Scene2D::RenderScene이 자동 호출)
	void SetOrthographic2D(_f32 _width, _f32 _height);

	// 2D 카메라 이동 (카메라가 비추는 중심이 이 월드 위치로 온다)
	void SetPosition2D(const vec2& _position);
	const vec2& Position2D() const { return position2D_; }

	// 현재 위치에서 상대 이동
	void Move2D(const vec2& _delta);

	// 2D 줌 (1보다 크면 확대, 작으면 축소)
	void SetZoom(_f32 _zoom);
	_f32 Zoom() const { return zoom_; }

	// 현재 줌에 배율을 곱한다. (휠 한 칸에 ZoomBy(1.1f) 식으로 사용)
	void ZoomBy(_f32 _scale);

	// 화면 픽셀 좌표(왼쪽 위 원점, Y아래+) → 월드 좌표 (클릭 픽에 사용)
	vec2 ScreenToWorld2D(_f32 _screenX, _f32 _screenY) const;

	// 월드 좌표 → 화면 픽셀 좌표 (머리 위 이름표 UI 등에 사용)
	vec2 WorldToScreen2D(const vec2& _world) const;

	// 표준 2D 카메라 조작 (방향키 이동 + 휠 줌) — 매 프레임 OnUpdate에서 호출
	void DriveDefault2D(InputManager& _input, const jc::TimeSpan& _dt);

	// 카메라가 구성되었는가? (SetOrthographic2D 호출 여부)
	bool IsConfigured() const { return configured_; }

	// === Camera 추상 구현 ===
	virtual mat4 View() const override { return view_; }
	virtual mat4 Projection() const override { return projection_; }

private:
	// 2D 설정값으로 뷰/투영 행렬을 다시 계산한다.
	void Rebuild2D();

private:
	bool configured_ = false;		// SetOrthographic2D 호출 여부
	mat4 view_;						// 뷰 행렬
	mat4 projection_;				// 투영 행렬
	_f32 width2D_;					// 2D 가상 화면 가로
	_f32 height2D_;					// 2D 가상 화면 세로
	vec2 position2D_;				// 2D 카메라 중심 위치
	_f32 zoom_ = 1.0f;				// 2D 줌 배율
};

NS_SGF_END