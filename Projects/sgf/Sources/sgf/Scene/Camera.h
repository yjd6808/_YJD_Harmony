/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:30:00 AM
 * 수정일: 8/9/2026 1:00:00 AM (v2: 일반 프로그래머용 편의 API 대폭 추가)
 * =====================
 * 카메라 (2D 직교 투영 / 3D 원근 투영)
 *
 * [카메라의 역할]
 *  카메라는 두 개의 행렬을 만든다.
 *  1. 뷰 행렬(View)       : 월드 좌표 -> 카메라 기준 좌표
 *     "카메라가 움직인다" = "세상 전체가 반대로 움직인다"와 같다.
 *  2. 투영 행렬(Projection) : 카메라 기준 좌표 -> 클립 좌표(NDC)
 *     - 직교(Orthographic): 멀리 있어도 같은 크기. 2D/UI용.
 *     - 원근(Perspective) : 멀수록 작게. 사람 눈과 같은 3D용.
 *
 * [v2에서 바뀐 점 - 왜 바꿨나?]
 *  기존에는 행렬 지식이 없으면 카메라를 다루기 어려웠다.
 *  v2는 "이동/줌/궤도회전/화면좌표 변환" 같은 일상 조작을 함수 하나로 제공한다.
 *   - 2D: Move2D / ZoomBy / ScreenToWorld2D / WorldToScreen2D / DriveDefault2D
 *   - 3D: SetPerspectiveDegrees / Orbit / Dolly / Pan / MoveForward / DriveDefault3D
 *  DriveDefault2D/3D는 키보드+마우스휠 표준 조작을 통째로 처리해주므로
 *  "카메라 조작이 되는 데모"를 한 줄로 만들 수 있다.
 */

#pragma once

#include "jc/Math.h"
#include "jc/Time.h"

NS_SGF_BEGIN

using namespace jc;

class InputManager;

class Camera
{
public:
	Camera();
	~Camera();

	// ==================== 2D 모드 ====================

	// 2D 직교 카메라로 설정한다.
	// 화면 왼쪽 아래가 (0,0), 오른쪽 위가 (_width,_height)인 좌표계.
	// (수학 교과서와 같은 Y위쪽+ 좌표계라 직관적이다)
	void SetOrthographic2D(_f32 _width, _f32 _height);

	// 2D 카메라 이동 (카메라가 비추는 중심이 이 월드 위치로 온다)
	void SetPosition2D(const Vec2& _position);
	const Vec2& Position2D() const { return m_Position2D; }

	// 2D 카메라를 현재 위치에서 _delta만큼 상대 이동한다.
	void Move2D(const Vec2& _delta);

	// 2D 줌 (1보다 크면 확대, 작으면 축소)
	void SetZoom(_f32 _zoom);
	_f32 Zoom() const { return m_Zoom; }

	// 현재 줌에 배율을 곱한다. (휠 한 칸에 ZoomBy(1.1f) 식으로 사용)
	void ZoomBy(_f32 _scale);

	// 마우스 커서 등 "화면 픽셀 좌표(왼쪽 위 원점, Y아래+)"를 월드 좌표로 변환한다.
	// 클릭한 곳에 물체를 놓는 기능을 만들 때 필수.
	Vec2 ScreenToWorld2D(_f32 _screenX, _f32 _screenY) const;

	// 월드 좌표를 화면 픽셀 좌표로 변환한다. (머리 위 이름표 UI 등에 사용)
	Vec2 WorldToScreen2D(const Vec2& _world) const;

	// 표준 2D 카메라 조작을 한 번에 처리한다. 매 프레임 OnUpdate에서 호출만 하면 된다.
	//  - 방향키: 카메라 이동 (줌이 클수록 화면상 속도가 일정하도록 보정)
	//  - 마우스 휠: 줌 인/아웃
	void DriveDefault2D(InputManager& _input, const jc::TimeSpan& _dt);

	// ==================== 3D 모드 ====================

	// 3D 원근 카메라로 설정한다.
	// @param _fovY   : 세로 시야각 (라디안). 보통 60도 = Pi/3
	// @param _aspect : 화면 가로/세로 비율
	// @param _nearZ  : 이보다 가까우면 안 그림 (0이면 안 됨! 보통 0.1)
	// @param _farZ   : 이보다 멀면 안 그림
	void SetPerspective(_f32 _fovY, _f32 _aspect, _f32 _nearZ, _f32 _farZ);

	// 시야각을 "도(degree)" 단위로 받는 편의 버전. (라디안 계산이 필요 없다)
	void SetPerspectiveDegrees(_f32 _fovYDegrees, _f32 _aspect, _f32 _nearZ, _f32 _farZ);

	// 3D 카메라 위치/바라보는 지점 설정
	void SetLookAt(const Vec3& _eye, const Vec3& _target, const Vec3& _up = Vec3(0.0f, 1.0f, 0.0f));
	const Vec3& Eye() const { return m_Eye; }
	const Vec3& Target() const { return m_Target; }

	// 바라보는 지점(Target)을 중심으로 카메라를 공전시킨다. (에디터의 Alt+드래그 회전)
	// @param _yawDelta   : 좌우 회전량 (라디안, +면 오른쪽으로)
	// @param _pitchDelta : 상하 회전량 (라디안, +면 위로. 수직 근처에서 자동 제한)
	void Orbit(_f32 _yawDelta, _f32 _pitchDelta);

	// 바라보는 지점을 향해 다가가거나(+) 멀어진다(-). (휠 줌에 해당)
	void Dolly(_f32 _distance);

	// 카메라의 오른쪽/위쪽 방향으로 시점과 목표점을 함께 평행 이동한다. (에디터의 팬)
	void Pan(_f32 _rightDelta, _f32 _upDelta);

	// 바라보는 방향 기준 이동 (FPS 스타일 WASD 이동의 재료)
	void MoveForward(_f32 _distance);
	void MoveRight(_f32 _distance);
	void MoveUp(_f32 _distance);

	// 표준 3D 궤도 카메라 조작을 한 번에 처리한다.
	//  - 방향키: Target 중심 공전(Orbit) / W,S: 접근·후퇴(Dolly)
	//  - A,D: 좌우 팬 / Q,E: 상하 팬 / 마우스 휠: Dolly
	void DriveDefault3D(InputManager& _input, const jc::TimeSpan& _dt);

	// ==================== 행렬 얻기 ====================

	const Mat4& View() const { return m_View; }
	const Mat4& Projection() const { return m_Projection; }

	// 뷰 x 투영 합성 행렬 (셰이더에 올리는 값)
	Mat4 ViewProjection() const { return m_View * m_Projection; }

private:
	// 2D 설정값으로 뷰/투영 행렬을 다시 계산한다.
	void Rebuild2D();

	// 현재 Eye/Target/Up으로 뷰 행렬을 다시 계산한다.
	void Rebuild3D();

private:
	Mat4 m_View;			// 뷰 행렬
	Mat4 m_Projection;		// 투영 행렬

	// 2D 상태
	bool m_b2D;				// 2D 모드인가?
	_f32 m_Width2D;			// 2D 가상 화면 가로
	_f32 m_Height2D;		// 2D 가상 화면 세로
	Vec2 m_Position2D;		// 2D 카메라 중심 위치
	_f32 m_Zoom;			// 2D 줌 배율

	// 3D 상태
	Vec3 m_Eye;				// 카메라 위치
	Vec3 m_Target;			// 바라보는 지점
	Vec3 m_Up;				// 위 방향
};

NS_SGF_END
