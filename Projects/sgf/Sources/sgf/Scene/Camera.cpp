/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:30:00 AM
 * 수정일: 8/9/2026 1:00:00 AM (v2: 편의 API 추가)
 * =====================
 * 카메라 구현부
 */

#include "Core.h"
#include "sgf/Scene/Camera.h"
#include "sgf/Input/InputManager.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Camera::Camera()
	: view_(mat4::Identity())
	, projection_(mat4::Identity())
	, b2D_(true)
	, width2D_(0.0f)
	, height2D_(0.0f)
	, position2D_(0.0f, 0.0f)
	, zoom_(1.0f)
	, eye_(0.0f, 0.0f, -5.0f)
	, target_(0.0f, 0.0f, 0.0f)
	, up_(0.0f, 1.0f, 0.0f)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Camera::~Camera()
{
}

// ==================== 2D 모드 ====================

//////////////////////////////////////////////////////////////////////////////////////////
// 2D 직교 카메라 설정. 기본 상태는 화면 왼쪽 아래 (0,0) ~ 오른쪽 위 (w,h).
void Camera::SetOrthographic2D(_f32 _width, _f32 _height)
{
	b2D_ = true;
	width2D_ = _width;
	height2D_ = _height;

	// 카메라 중심을 화면 한가운데에 맞추면 월드 (0,0)이 정확히 화면 왼쪽 아래가 된다.
	position2D_ = vec2(_width * 0.5f, _height * 0.5f);
	zoom_ = 1.0f;
	Rebuild2D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 2D 카메라 이동 (카메라 중심이 이 위치로 온다). 2D 모드에서만 유효하다. (3D 모드 no-op)
void Camera::SetPosition2D(const vec2& _position)
{
	if (!b2D_)
	{
		return;
	}
	if (position2D_.x != _position.x || position2D_.y != _position.y)
	{
		position2D_ = _position;
		Rebuild2D();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 현재 위치 기준 상대 이동
void Camera::Move2D(const vec2& _delta)
{
	SetPosition2D(position2D_ + _delta);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 2D 줌 설정 (0 이하 방지). 2D 모드에서만 유효하다. (3D 모드 no-op)
void Camera::SetZoom(_f32 _zoom)
{
	if (!b2D_)
	{
		return;
	}
	if (_zoom < 0.0001f)
	{
		_zoom = 0.0001f;
	}
	if (zoom_ != _zoom)
	{
		zoom_ = _zoom;
		Rebuild2D();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 현재 줌에 배율 곱하기
void Camera::ZoomBy(_f32 _scale)
{
	SetZoom(zoom_ * _scale);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 화면 픽셀 좌표(왼쪽 위 원점, Y아래+) -> 월드 좌표
// [원리] 그리기는 "월드 -> (카메라 중심 빼기) -> (줌 곱하기) -> 화면" 순서다.
// 그 과정을 정확히 거꾸로 되짚으면 마우스 아래의 월드 좌표가 나온다.
vec2 Camera::ScreenToWorld2D(_f32 _screenX, _f32 _screenY) const
{
	const _f32 halfW = width2D_ * 0.5f;
	const _f32 halfH = height2D_ * 0.5f;

	// 1. 픽셀 좌표를 "화면 중심 기준, Y위쪽+" 좌표로 바꾼다.
	const _f32 camX = _screenX - halfW;
	const _f32 camY = (height2D_ - _screenY) - halfH;

	// 2. 줌을 되돌리고 카메라 중심을 더하면 월드 좌표다.
	return vec2(position2D_.x + camX / zoom_, position2D_.y + camY / zoom_);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 월드 좌표 -> 화면 픽셀 좌표 (ScreenToWorld2D의 역변환)
vec2 Camera::WorldToScreen2D(const vec2& _world) const
{
	const _f32 halfW = width2D_ * 0.5f;
	const _f32 halfH = height2D_ * 0.5f;

	const _f32 camX = (_world.x - position2D_.x) * zoom_;
	const _f32 camY = (_world.y - position2D_.y) * zoom_;

	return vec2(camX + halfW, height2D_ - (camY + halfH));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 표준 2D 카메라 조작 (방향키 이동 + 휠 줌)
void Camera::DriveDefault2D(InputManager& _input, const jc::TimeSpan& _dt)
{
	const _f32 dt = static_cast<_f32>(_dt.GetTotalSeconds());

	// 줌인 상태에서도 "화면상 속도"가 일정하도록 줌으로 나눠 보정한다.
	const _f32 speed = 300.0f / zoom_;

	vec2 move(0.0f, 0.0f);
	if (_input.IsKeyDown(VK_LEFT)) { move.x -= 1.0f; }
	if (_input.IsKeyDown(VK_RIGHT)) { move.x += 1.0f; }
	if (_input.IsKeyDown(VK_DOWN)) { move.y -= 1.0f; }
	if (_input.IsKeyDown(VK_UP)) { move.y += 1.0f; }

	if (move.LengthSquared() > 0.0f)
	{
		Move2D(move.Normalized() * speed * dt);
	}

	// 마우스 휠: 한 칸당 1.1배 줌 인/아웃 (WheelDelta는 이미 틱 단위, ±1 = 한 칸)
	const int wheel = _input.WheelDelta();
	if (wheel != 0)
	{
		ZoomBy(powf(1.1f, static_cast<_f32>(wheel)));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 2D 뷰/투영 행렬 재계산
void Camera::Rebuild2D()
{
	// 뷰: "카메라가 움직인다 = 세상이 반대로 움직인다"
	// 월드에서 카메라 중심을 뺀 뒤 줌 배율을 곱한다.
	view_ = mat4::Translation(-position2D_.x, -position2D_.y, 0.0f)
		* mat4::Scale(zoom_, zoom_, 1.0f);

	// 투영: 화면 중심 기준 -half ~ +half 범위를 NDC(-1~+1)로 매핑
	const _f32 halfW = width2D_ * 0.5f;
	const _f32 halfH = height2D_ * 0.5f;
	projection_ = mat4::OrthographicOffCenterLH(-halfW, +halfW, -halfH, +halfH, 0.0f, 1.0f);
}

NS_SGF_END

// ==================== 3D 모드 ====================
// (이 아래 함수들은 편의상 NS_SGF 내부에 이어서 정의한다)

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
// 3D 원근 투영 설정 (라디안)
void Camera::SetPerspective(_f32 _fovY, _f32 _aspect, _f32 _nearZ, _f32 _farZ)
{
	b2D_ = false;
	projection_ = mat4::PerspectiveFovLH(_fovY, _aspect, _nearZ, _farZ);
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 3D 원근 투영 설정 (도 단위 - 일반 프로그래머용 편의 버전)
void Camera::SetPerspectiveDegrees(_f32 _fovYDegrees, _f32 _aspect, _f32 _nearZ, _f32 _farZ)
{
	SetPerspective(jc_math_deg2rad(_fovYDegrees), _aspect, _nearZ, _farZ);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 카메라 위치/바라보는 지점 설정
void Camera::SetLookAt(const vec3& _eye, const vec3& _target, const vec3& _up)
{
	b2D_ = false;
	eye_ = _eye;
	target_ = _target;
	up_ = _up;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Target 중심 공전 (에디터의 Alt+드래그 회전 느낌)
// [원리] 카메라 위치를 구면 좌표(반지름 + 가로각 yaw + 세로각 pitch)로 바꿔서
//  각도만 더한 뒤 다시 직교 좌표로 되돌린다.
void Camera::Orbit(_f32 _yawDelta, _f32 _pitchDelta)
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
// 목표점을 향해 접근(+)/후퇴(-)
void Camera::Dolly(_f32 _distance)
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
// 카메라 기준 오른쪽/위쪽으로 시점+목표점 함께 평행 이동 (팬)
void Camera::Pan(_f32 _rightDelta, _f32 _upDelta)
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
// 바라보는 방향으로 전진/후진 (시점+목표점 함께)
void Camera::MoveForward(_f32 _distance)
{
	const vec3 forward = (target_ - eye_).Normalized();
	eye_ = eye_ + forward * _distance;
	target_ = target_ + forward * _distance;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 카메라 오른쪽 방향으로 이동
void Camera::MoveRight(_f32 _distance)
{
	const vec3 forward = (target_ - eye_).Normalized();
	const vec3 right = up_.Cross(forward).Normalized();
	eye_ = eye_ + right * _distance;
	target_ = target_ + right * _distance;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 월드 위쪽 방향으로 이동
void Camera::MoveUp(_f32 _distance)
{
	const vec3 up = up_.Normalized();
	eye_ = eye_ + up * _distance;
	target_ = target_ + up * _distance;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 표준 3D 궤도 카메라 조작
void Camera::DriveDefault3D(InputManager& _input, const jc::TimeSpan& _dt)
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
	const int wheel = _input.WheelDelta();
	if (wheel != 0)
	{
		Dolly(static_cast<_f32>(wheel) * 0.5f);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 3D 뷰 행렬 재계산
void Camera::Rebuild3D()
{
	view_ = mat4::LookAtLH(eye_, target_, up_);
}

NS_SGF_END
