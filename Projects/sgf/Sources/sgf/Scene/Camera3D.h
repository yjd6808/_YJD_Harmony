/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (Camera를 GameObject 파생으로 분리 — 3D 전용)
 * =====================
 * Camera3D — 원근 카메라 (GameObject 파생)
 *
 * [역할]
 * 3D 원근 투영 카메라. 멀수록 작게 보인다. (사람 눈과 같은 3D용)
 * - SetPerspectiveDegrees: 구성 (렌더 전 1회)
 * - SetLookAt: 위치/바라보는 지점 설정
 * - Orbit/Dolly/Pan: 궤도 회전/접근/평행 이동
 * - DriveDefault3D: 표준 궤도 카메라 조작 (매 프레임 1줄)
 *
 * [변경]
 * Camera(GameObject 파생)를 상속 → Transform/GID/DataMap 자동 보유, 트리 배치 가능.
 * Scene::CreateDefaultCamera(Camera3D::CreateDefault())로 생성·소유된다.
 */

#pragma once

#include "jc/Math.h"
#include "jc/Time.h"
#include "sgf/Scene/Camera.h"

NS_SGF_BEGIN

using namespace jc;

class InputManager;

class Camera3D : public Camera
{
public:
	Camera3D();

	// [팩토리] 기본 3D 카메라 생성 — 이름 "DefaultCamera3D"의 GameObject (자체가 카메라)
	static Camera3D* CreateDefault();

	// 3D 원근 투영 설정 (라디안)
	void SetPerspective(_f32 _fovY, _f32 _aspect, _f32 _nearZ, _f32 _farZ);

	// 3D 원근 투영 설정 (도 단위 — 일반 프로그래머용 편의 버전)
	void SetPerspectiveDegrees(_f32 _fovYDegrees, _f32 _aspect, _f32 _nearZ, _f32 _farZ);

	// 카메라 위치/바라보는 지점 설정
	void SetLookAt(const vec3& _eye, const vec3& _target, const vec3& _up = vec3(0.0f, 1.0f, 0.0f));
	const vec3& Eye() const { return eye_; }
	const vec3& Target() const { return target_; }

	// 바라보는 지점(Target)을 중심으로 공전 (에디터 Alt+드래그 회전)
	void Orbit(_f32 _yawDelta, _f32 _pitchDelta);

	// 바라보는 지점을 향해 다가가거나(+) 멀어진다(-) (휠 줌)
	void Dolly(_f32 _distance);

	// 카메라 기준 오른쪽/위쪽으로 시점+목표점 함께 평행 이동 (팬)
	void Pan(_f32 _rightDelta, _f32 _upDelta);

	// 바라보는 방향 기준 이동 (FPS 스타일 WASD 이동의 재료)
	void MoveForward(_f32 _distance);
	void MoveRight(_f32 _distance);
	void MoveUp(_f32 _distance);

	// 표준 3D 궤도 카메라 조작 (방향키 공전, W/S 접근·후퇴, A/D·Q/E 팬, 휠 Dolly)
	void DriveDefault3D(InputManager& _input, const jc::TimeSpan& _dt);

	// 카메라가 구성되었는가? (SetPerspective* 호출 여부)
	bool IsConfigured() const { return configured_; }

	// === Camera 추상 구현 ===
	virtual mat4 View() const override { return view_; }
	virtual mat4 Projection() const override { return projection_; }

private:
	// 현재 Eye/Target/Up으로 뷰 행렬을 다시 계산한다.
	void Rebuild3D();

private:
	bool configured_ = false;		// SetPerspective* 호출 여부
	mat4 view_;						// 뷰 행렬
	mat4 projection_;				// 투영 행렬
	_f32 fovY_ = 0.0f;				// 세로 시야각 (라디안)
	_f32 aspect_ = 1.0f;			// 화면 가로/세로 비율
	_f32 nearZ_ = 0.1f;				// 근평면
	_f32 farZ_ = 1000.0f;			// 원평면
	vec3 eye_;						// 카메라 위치
	vec3 target_;					// 바라보는 지점
	vec3 up_;						// 위 방향
};

NS_SGF_END