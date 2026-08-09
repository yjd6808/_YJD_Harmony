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
	: m_View(Mat4::Identity())
	, m_Projection(Mat4::Identity())
	, m_b2D(true)
	, m_Width2D(0.0f)
	, m_Height2D(0.0f)
	, m_Position2D(0.0f, 0.0f)
	, m_Zoom(1.0f)
	, m_Eye(0.0f, 0.0f, -5.0f)
	, m_Target(0.0f, 0.0f, 0.0f)
	, m_Up(0.0f, 1.0f, 0.0f)
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
	m_b2D = true;
	m_Width2D = _width;
	m_Height2D = _height;

	// 카메라 중심을 화면 한가운데에 맞추면 월드 (0,0)이 정확히 화면 왼쪽 아래가 된다.
	m_Position2D = Vec2(_width * 0.5f, _height * 0.5f);
	m_Zoom = 1.0f;
	Rebuild2D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 2D 카메라 이동 (카메라 중심이 이 위치로 온다). 2D 모드에서만 유효하다. (3D 모드 no-op)
void Camera::SetPosition2D(const Vec2& _position)
{
	if (!m_b2D)
	{
		return;
	}
	if (m_Position2D.x != _position.x || m_Position2D.y != _position.y)
	{
		m_Position2D = _position;
		Rebuild2D();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 현재 위치 기준 상대 이동
void Camera::Move2D(const Vec2& _delta)
{
	SetPosition2D(m_Position2D + _delta);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 2D 줌 설정 (0 이하 방지). 2D 모드에서만 유효하다. (3D 모드 no-op)
void Camera::SetZoom(_f32 _zoom)
{
	if (!m_b2D)
	{
		return;
	}
	if (_zoom < 0.0001f)
	{
		_zoom = 0.0001f;
	}
	if (m_Zoom != _zoom)
	{
		m_Zoom = _zoom;
		Rebuild2D();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 현재 줌에 배율 곱하기
void Camera::ZoomBy(_f32 _scale)
{
	SetZoom(m_Zoom * _scale);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 화면 픽셀 좌표(왼쪽 위 원점, Y아래+) -> 월드 좌표
// [원리] 그리기는 "월드 -> (카메라 중심 빼기) -> (줌 곱하기) -> 화면" 순서다.
// 그 과정을 정확히 거꾸로 되짚으면 마우스 아래의 월드 좌표가 나온다.
Vec2 Camera::ScreenToWorld2D(_f32 _screenX, _f32 _screenY) const
{
	const _f32 halfW = m_Width2D * 0.5f;
	const _f32 halfH = m_Height2D * 0.5f;

	// 1. 픽셀 좌표를 "화면 중심 기준, Y위쪽+" 좌표로 바꾼다.
	const _f32 camX = _screenX - halfW;
	const _f32 camY = (m_Height2D - _screenY) - halfH;

	// 2. 줌을 되돌리고 카메라 중심을 더하면 월드 좌표다.
	return Vec2(m_Position2D.x + camX / m_Zoom, m_Position2D.y + camY / m_Zoom);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 월드 좌표 -> 화면 픽셀 좌표 (ScreenToWorld2D의 역변환)
Vec2 Camera::WorldToScreen2D(const Vec2& _world) const
{
	const _f32 halfW = m_Width2D * 0.5f;
	const _f32 halfH = m_Height2D * 0.5f;

	const _f32 camX = (_world.x - m_Position2D.x) * m_Zoom;
	const _f32 camY = (_world.y - m_Position2D.y) * m_Zoom;

	return Vec2(camX + halfW, m_Height2D - (camY + halfH));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 표준 2D 카메라 조작 (방향키 이동 + 휠 줌)
void Camera::DriveDefault2D(InputManager& _input, const jc::TimeSpan& _dt)
{
	const _f32 dt = static_cast<_f32>(_dt.GetTotalSeconds());

	// 줌인 상태에서도 "화면상 속도"가 일정하도록 줌으로 나눠 보정한다.
	const _f32 speed = 300.0f / m_Zoom;

	Vec2 move(0.0f, 0.0f);
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
	m_View = Mat4::Translation(-m_Position2D.x, -m_Position2D.y, 0.0f)
		* Mat4::Scale(m_Zoom, m_Zoom, 1.0f);

	// 투영: 화면 중심 기준 -half ~ +half 범위를 NDC(-1~+1)로 매핑
	const _f32 halfW = m_Width2D * 0.5f;
	const _f32 halfH = m_Height2D * 0.5f;
	m_Projection = Mat4::OrthographicOffCenterLH(-halfW, +halfW, -halfH, +halfH, 0.0f, 1.0f);
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
	m_b2D = false;
	m_Projection = Mat4::PerspectiveFovLH(_fovY, _aspect, _nearZ, _farZ);
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
void Camera::SetLookAt(const Vec3& _eye, const Vec3& _target, const Vec3& _up)
{
	m_b2D = false;
	m_Eye = _eye;
	m_Target = _target;
	m_Up = _up;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Target 중심 공전 (에디터의 Alt+드래그 회전 느낌)
// [원리] 카메라 위치를 구면 좌표(반지름 + 가로각 yaw + 세로각 pitch)로 바꿔서
//  각도만 더한 뒤 다시 직교 좌표로 되돌린다.
void Camera::Orbit(_f32 _yawDelta, _f32 _pitchDelta)
{
	Vec3 offset = m_Eye - m_Target;
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
	const _f32 kPitchLimit_v = jc_math_pi_half - 0.01f;
	pitch = Clamp(pitch, -kPitchLimit_v, +kPitchLimit_v);

	// 구면 좌표 -> 직교 좌표 복원
	const _f32 cosPitch = cosf(pitch);
	offset = Vec3(radius * cosPitch * sinf(yaw), radius * sinf(pitch), radius * cosPitch * cosf(yaw));

	m_Eye = m_Target + offset;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 목표점을 향해 접근(+)/후퇴(-)
void Camera::Dolly(_f32 _distance)
{
	Vec3 forward = m_Target - m_Eye;
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

	m_Eye = m_Eye + forward.Normalized() * moveAmount;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 카메라 기준 오른쪽/위쪽으로 시점+목표점 함께 평행 이동 (팬)
void Camera::Pan(_f32 _rightDelta, _f32 _upDelta)
{
	const Vec3 forward = (m_Target - m_Eye).Normalized();
	const Vec3 right = m_Up.Cross(forward).Normalized();	// LH: Up x Forward = Right
	const Vec3 up = forward.Cross(right);					// 카메라 기준 실제 위쪽

	const Vec3 delta = right * _rightDelta + up * _upDelta;
	m_Eye = m_Eye + delta;
	m_Target = m_Target + delta;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 바라보는 방향으로 전진/후진 (시점+목표점 함께)
void Camera::MoveForward(_f32 _distance)
{
	const Vec3 forward = (m_Target - m_Eye).Normalized();
	m_Eye = m_Eye + forward * _distance;
	m_Target = m_Target + forward * _distance;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 카메라 오른쪽 방향으로 이동
void Camera::MoveRight(_f32 _distance)
{
	const Vec3 forward = (m_Target - m_Eye).Normalized();
	const Vec3 right = m_Up.Cross(forward).Normalized();
	m_Eye = m_Eye + right * _distance;
	m_Target = m_Target + right * _distance;
	Rebuild3D();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 월드 위쪽 방향으로 이동
void Camera::MoveUp(_f32 _distance)
{
	const Vec3 up = m_Up.Normalized();
	m_Eye = m_Eye + up * _distance;
	m_Target = m_Target + up * _distance;
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
	m_View = Mat4::LookAtLH(m_Eye, m_Target, m_Up);
}

NS_SGF_END
