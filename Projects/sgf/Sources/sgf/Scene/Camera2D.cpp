/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (Camera를 GameObject 파생으로 분리 — 2D 구현부)
 * =====================
 * Camera2D 구현부 (기존 Camera.cpp의 2D 부분 분리)
 */

#include "Core.h"
#include "sgf/Scene/Camera2D.h"
#include "sgf/Input/InputManager.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Camera2D::Camera2D()
	: view_(mat4::Identity())
	, projection_(mat4::Identity())
	, width2D_(0.0f)
	, height2D_(0.0f)
	, position2D_(0.0f, 0.0f)
{
	SetName("DefaultCamera2D");
}

//////////////////////////////////////////////////////////////////////////////////////////
Camera2D* Camera2D::CreateDefault()
{
	return dbg_new Camera2D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera2D::SetOrthographic2D(_f32 _width, _f32 _height)
{
	width2D_ = _width;
	height2D_ = _height;

	// 카메라 중심을 화면 한가운데에 맞추면 월드 (0,0)이 정확히 화면 왼쪽 아래가 된다.
	position2D_ = vec2(_width * 0.5f, _height * 0.5f);
	zoom_ = 1.0f;
	configured_ = true;
	Rebuild2D();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera2D::SetPosition2D(const vec2& _position)
{
	if (position2D_.x != _position.x || position2D_.y != _position.y)
	{
		position2D_ = _position;
		Rebuild2D();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera2D::Move2D(const vec2& _delta)
{
	SetPosition2D(position2D_ + _delta);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera2D::SetZoom(_f32 _zoom)
{
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
void Camera2D::ZoomBy(_f32 _scale)
{
	SetZoom(zoom_ * _scale);
}

//////////////////////////////////////////////////////////////////////////////////////////
vec2 Camera2D::ScreenToWorld2D(_f32 _screenX, _f32 _screenY) const
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
vec2 Camera2D::WorldToScreen2D(const vec2& _world) const
{
	const _f32 halfW = width2D_ * 0.5f;
	const _f32 halfH = height2D_ * 0.5f;

	const _f32 camX = (_world.x - position2D_.x) * zoom_;
	const _f32 camY = (_world.y - position2D_.y) * zoom_;

	return vec2(camX + halfW, height2D_ - (camY + halfH));
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera2D::DriveDefault2D(InputManager& _input, const jc::TimeSpan& _dt)
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
	const _s32 wheel = _input.WheelDelta();
	if (wheel != 0)
	{
		ZoomBy(powf(1.1f, static_cast<_f32>(wheel)));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera2D::Rebuild2D()
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