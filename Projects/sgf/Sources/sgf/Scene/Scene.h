/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:34:00 AM
 * 수정일: 8/9/2026 10:00:00 AM (v2.1: OnRender 인자 제거 + 소속 윈도우 참조)
 * =====================
 * 씬(Scene)
 *
 * [씬이란?]
 *  게임의 "화면 한 장". 타이틀 화면, 인게임 화면, 결과 화면 등.
 *  Cocos2d-x의 cocos2d::Scene과 같은 역할이다.
 *  Director가 윈도우별로 씬을 들고 있으며, 매 프레임 OnUpdate/OnRender를 불러준다.
 *
 * [v2.1에서 바뀜 점]
 *  1. OnRender()에서 Renderer2D* 인자가 사라졌다.
 *     렌더러는 전역 매크로(g_cRenderer2D / g_cRenderer3D)로 언제든 접근한다.
 *     -> 씬 코드가 특정 렌더러 인자에 종속되지 않고, 2D/3D를 자유롭게 섞어 그린다.
 *  2. 씬은 자신이 그려지는 윈도우를 안다. (GetWindow)
 *     -> 화면 크기 기반 카메라 설정 등이 가능해진다.
 *     (Director가 씬을 윈도우 슬롯에 올릴 때 자동으로 설정해준다)
 *
 * [사용법]
 *  class MyScene : public sgf::Scene
 *  {
 *      void OnEnter() override
 *      {
 *          // 소속 윈도우 크기에 맞춰 카메라 설정 (v2.1)
 *          camera_.SetOrthographic2D(_f32(GetWindow()->Width()), _f32(GetWindow()->Height()));
 *      }
 *      void OnUpdate(const jc::TimeSpan& _dt) override { ... }
 *      void OnRender() override
 *      {
 *          g_cRenderer2D.DrawRect(...);      // 2D 배치
 *          g_cRenderer3D.DrawCube(...);      // 3D 배치 (같은 프레임에 섞어도 된다)
 *      }
 *      void OnExit() override { ... }
 *  };
 *  g_cDirector.RunScene(new MyScene());
 */

#pragma once

#include "sgf/Scene/Camera.h"

NS_SGF_BEGIN

class Director;
class Window;

class Scene
{
public:
	Scene() : pWindow_(nullptr) {}
	virtual ~Scene() {}

	// === 생명주기 훅 (파생 클래스가 재정의) ===

	// 씬이 화면에 등장할 때 1회 호출. 리소스 로딩을 여기서 한다.
	// (이 시점부터 GetWindow()가 유효하다)
	virtual void OnEnter() {}

	// 씬이 화면에서 내려갈 때 1회 호출. 리소스 정리를 여기서 한다.
	virtual void OnExit() {}

	// 매 프레임 로직 갱신. _dt는 직전 프레임으로부터 경과한 시간이다.
	// (jc::TimeSpan이므로 _dt.GetTotalSeconds()로 초 단위 실수를 얻는다)
	virtual void OnUpdate(const jc::TimeSpan& _dt) { (void)_dt; }

	// 매 프레임 그리기.
	// [v2.1] 인자가 없다! g_cRenderer2D / g_cRenderer3D 매크로로 그린다.
	// Director가 씬 카메라 행렬로 두 배치의 Begin/End를 감싸서 불러준다.
	virtual void OnRender() {}

	// 이 씬의 카메라 (Director가 렌더링 시 ViewProjection을 가져다 쓴다)
	Camera& GetCamera() { return camera_; }

	// [v2.1] 이 씬이 그려지는 윈도우.
	// Director가 RunScene/ReplaceScene 시점에 설정해준다.
	// OnEnter 이후부터 유효하다. (그 전에는 nullptr)
	Window* GetWindow() const { return pWindow_; }

protected:
	Camera camera_;	// 씬 전용 카메라

private:
	// Director만 소속 윈도우를 바꿀 수 있다.
	friend class Director;
	void SetWindow(Window* _pWindow) { pWindow_ = _pWindow; }

private:
	Window* pWindow_;	// 이 씬이 그려지는 창 (빌림, 소유 아님)
};

NS_SGF_END
