/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (Camera를 GameObject 파생으로 분리 — 3D 구현부)
 * =====================
 * Camera3D 구현부 (기존 Camera.cpp의 3D 부분 분리)
 */

#include "Core.h"
#include "sgf/Scene/Camera3D.h"
#include "sgf/Input/InputManager.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Camera3D::Camera3D()
	: view_(mat4::Identity())
	, projection_(mat4::Identity())
	, eye_(0.0f, 0.0f, -5.0f)
	, target_(0.0f, 0.0f, 0.0f)
	, up_(0.0f, 1.0f, 0.0f)
{
	SetName("DefaultCamera3D");
}

//////////////////////////////////////////////////////////////////////////////////////////
Camera3D* Camera3D::CreateDefault()
{
	return dbg_new Camera3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::SetPerspective(_f32 _fovY, _f32 _aspect, _f32 _nearZ, _f32 _farZ)
{
	fovY_ = _fovY;
	aspect_ = _aspect;
	nearZ_ = _nearZ;
	farZ_ = _farZ;
	projection_ = mat4::PerspectiveFovLH(_fovY, _aspect, _nearZ, _farZ);
	configured_ = true;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::SetPerspectiveDegrees(_f32 _fovYDegrees, _f32 _aspect, _f32 _nearZ, _f32 _farZ)
{
	SetPerspective(jc_math_deg2rad(_fovYDegrees), _aspect, _nearZ, _farZ);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::SetLookAt(const vec3& _eye, const vec3& _target, const vec3& _up)
{
	eye_ = _eye;
	target_ = _target;
	up_ = _up;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Target 중심 공전 (에디터의 Alt+드래그 회전 느낌)
// [원리] 카메라 위치를 구면 좌표(반지름 + 가로각 yaw + 세로각 pitch)로 바꿔서
// 각도만 더한 뒤 다시 직교 좌표로 되돌린다.
void Camera3D::Orbit(_f32 _yawDelta, _f32 _pitchDelta)
{
	vec3 offset = eye_ - target_;
	const _f32 radius = offset.Length();
	if (radius < 0.0001f)
	{
		return;	// 시점과 목표가 겹치면 회전 방향을 정의할 수 없다.
	}

	// 현재 각도 복원 (LH 기준: yaw는 XZ평면, pitch는 상하각)
	_f32 yaw = atan2f(offset.x, offset.z);
	_f32 pitch = asinf(Clamp(offset.y / radius, -1.0f, 1.0f));

	yaw += _yawDelta;
	pitch += _pitchDelta;

	// 수직(진지/천정)을 정확히 넘어가면 카메라가 뒤집힌다. 살짝 못 미치게 제한.
	const _f32 PITCH_LIMIT = jc_math_pi_half - 0.01f;
	pitch = Clamp(pitch, -PITCH_LIMIT, +PITCH_LIMIT);

	// 구면 좌표 -> 직교 좌표 복원
	const _f32 cosPitch = cosf(pitch);
	offset = vec3(radius * cosPitch * sinf(yaw), radius * sinf(pitch), radius * cosPitch * cosf(yaw));

	eye_ = target_ + offset;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::Dolly(_f32 _distance)
{
	vec3 forward = target_ - eye_;
	const _f32 dist = forward.Length();
	if (dist < 0.0001f)
	{
		return;
	}

	// 목표점을 붙고 지나가지 않도록 최소 거리 0.1을 남긴다. (전진일 때만 제한, 후퇴는 항상 허용)
	_f32 moveAmount = _distance;
	const _f32 minDistance = 0.1f;
	if (moveAmount > 0.0f && moveAmount > dist - minDistance)
	{
		moveAmount = (dist - minDistance > 0.0f) ? (dist - minDistance) : 0.0f;
	}

	eye_ = eye_ + forward.Normalized() * moveAmount;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::Pan(_f32 _rightDelta, _f32 _upDelta)
{
	const vec3 forward = (target_ - eye_).Normalized();
	const vec3 right = up_.Cross(forward).Normalized();	// LH: Up x Forward = Right
	const vec3 up = forward.Cross(right);					// 카메라 기준 실제 위쪽

	const vec3 delta = right * _rightDelta + up * _upDelta;
	eye_ = eye_ + delta;
	target_ = target_ + delta;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::MoveForward(_f32 _distance)
{
	const vec3 forward = (target_ - eye_).Normalized();
	eye_ = eye_ + forward * _distance;
	target_ = target_ + forward * _distance;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::MoveRight(_f32 _distance)
{
	const vec3 forward = (target_ - eye_).Normalized();
	const vec3 right = up_.Cross(forward).Normalized();
	eye_ = eye_ + right * _distance;
	target_ = target_ + right * _distance;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::MoveUp(_f32 _distance)
{
	const vec3 up = up_.Normalized();
	eye_ = eye_ + up * _distance;
	target_ = target_ + up * _distance;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::DriveDefault3D(InputManager& _input, const jc::TimeSpan& _dt)
{
	const _f32 dt = static_cast<_f32>(_dt.GetTotalSeconds());
	const _f32 orbitSpeed = 1.5f;	// 라디안/초
	const _f32 moveSpeed = 5.0f;	// 월드 유닛/초

	// 방향키: Target 중심 공전
	if (_input.IsKeyDown(VK_LEFT)) { Orbit(-orbitSpeed * dt, 0.0f); }
	if (_input.IsKeyDown(VK_RIGHT)) { Orbit(+orbitSpeed * dt, 0.0f); }
	if (_input.IsKeyDown(VK_UP)) { Orbit(0.0f, +orbitSpeed * dt); }
	if (_input.IsKeyDown(VK_DOWN)) { Orbit(0.0f, -orbitSpeed * dt); }

	// W/S: 접근/후퇴, A/D: 좌우 팬, Q/E: 상하 팬
	if (_input.IsKeyDown('W')) { Dolly(+moveSpeed * dt); }
	if (_input.IsKeyDown('S')) { Dolly(-moveSpeed * dt); }
	if (_input.IsKeyDown('A')) { Pan(-moveSpeed * dt, 0.0f); }
	if (_input.IsKeyDown('D')) { Pan(+moveSpeed * dt, 0.0f); }
	if (_input.IsKeyDown('Q')) { Pan(0.0f, -moveSpeed * dt); }
	if (_input.IsKeyDown('E')) { Pan(0.0f, +moveSpeed * dt); }

	// 마우스 휠: 한 칸당 0.5유닛 접근/후퇴 (WheelDelta는 이미 틱 단위, ±1 = 한 칸)
	const _s32 wheel = _input.WheelDelta();
	if (wheel != 0)
	{
		Dolly(static_cast<_f32>(wheel) * 0.5f);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera3D::Rebuild3D()
{
	view_ = mat4::LookAtLH(eye_, target_, up_);

	// 뷰/투영이 바뀌었으니 VP 캐시 무효화 (B-6)
	// (SetPerspective는 projection_을 직접 설정하지만 Rebuild3D를 호출하므로 함께 커버된다)
	InvalidateViewProjection();
}

NS_SGF_END