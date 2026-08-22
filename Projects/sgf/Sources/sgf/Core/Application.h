/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:42:00 AM
 * 수정일: 8/9/2026 10:10:00 AM (멀티 윈도우 + Renderer3D 전역 1개)
 * =====================
 * 애플리케이션 (엔진 총괄)
 *
 * [역할]
 * 윈도우/그래픽 디바이스/입력/렌더러/타이머/사운드를 한데 묶어
 * "게임 루프"를 돌려주는 엔진의 입구 클래스.
 * Cocos2d-x의 Application(AppDelegate)과 같은 위치다.
 *
 * [바뀜 점]
 * 1. 렌더러가 2개가 됐다: Renderer2D + Renderer3D.
 * 둘 다 "전역으로 단 1개"를 Application이 소유한다.
 * [왜 씬별/창별이 아니라 전역 1개인가?]
 * - 렌더러의 알맹이(셰이더/정점버퍼)는 "디바이스" 소속이다.
 * 디바이스는 앱 전체에 1개뿐이므로 씬/창마다 복제하면 순수 낭비다.
 * - DX11 즉시 컨텍스트는 1개라 어차피 동시에 하나만 그린다.
 * - 배치 버퍼도 그리고 비우면 재사용되므로 공유가 가능하다.
 * 대신 "어디에 그릴지(창)"와 "어떤 카메라로 볼지(씬)"는
 * BeginFrame(Window*)와 Renderer::Begin(행렬)이 매 프레임 정해준다.
 * 2. 윈도우를 여러 개 가질 수 있다. (CreateSubWindow)
 * 메인 윈도우가 닫히면 앱 종료, 서브 윈도우는 그 창만 닫힌다.
 * 서브 윈도우에 씬을 올리려면 g_cDirector.RunScene(pScene, pSubWindow).
 *
 * [게임 루프란?]
 * 게임은 아래 단계를 1초에 수십 번(보통 60번) 반복하는 프로그램이다.
 * 1. 입력 처리 (윈도우 메시지 펌프)
 * 2. 갱신 (씬/앱 로직, jc::TimeSpan dt만큼 진행)
 * 3. 그리기 (창마다: 화면 지우기 -> 씬 그리기 -> 화면 표시)
 * 4. 사운드 엔진 정리 (끝난 소리 회수)
 *
 * [사용법 - 최소 코드]
 * class MyApp: public sgf::Application
 * {
 * bool ApplicationDidFinishLaunching() override
 * {
 * g_cDirector.RunScene(new MyScene());
 * return true;
 * }
 * };
 * MyApp app;
 * if (app.Initialize("제목", 1280, 720)) { app.Run(); }
 * app.Finalize();
 */

#pragma once

#include "jc/Primitives/String.h"
#include "sgf/Core/Window.h"
#include "sgf/Core/FrameTimer.h"
#include "sgf/Input/InputManager.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/Renderer2D.h"
#include "sgf/Graphics/Renderer3D.h"

#include <jc/Container/Vector.h>

NS_SGF_BEGIN

using namespace jc;

class Application
{
public:
	Application();
	virtual ~Application();

	// 엔진 초기화: 윈도우 -> DX11 디바이스 -> 창 표면 -> 렌더러 -> 사운드 순서로
	// 준비한 뒤 ApplicationDidFinishLaunching 훅을 부른다.
	// @param _title: 메인 윈도우 제목 (UTF-8)
	// @param _width: 클라이언트 영역 가로 크기
	// @param _height: 클라이언트 영역 세로 크기
	// @return 성공 여부. 실패 시 디버그 출력 창에 원인이 출력된다.
	bool Initialize(const jc::String& _title, _s32 _width, _s32 _height);

	// 게임 루프 실행: 메인 윈도우가 닫힐 때까지 돌아온다.
	void Run();

	// 엔진 종료: 씬/서브 윈도우/사운드/렌더러/디바이스 순서로 정리한다.
	void Finalize();

	// 서브 윈도우 생성.
	// 만들어진 창은 Application이 소유하며, 창이 닫히거나 앱이 끝날 때 자동 정리된다.
	// 서브 윈도우는 닫혀도 앱이 종료되지 않는다. (메인 윈도우만 앱 종료를 유발)
	// 키보드/마우스 입력은 메인 윈도우에만 연결된다.
	// @return 생성된 윈도우. 실패 시 nullptr.
	Window* CreateSubWindow(const jc::String& _title, _s32 _width, _s32 _height);

	////////////////////////////////////////////////////////////////////////////////////////
	// 서브시스템 접근자 (아래 g_c 매크로가 사용한다)
	Window& GetWindow() { return window_; }
	GraphicDevice& GetDevice() { return device_; }
	InputManager& GetInput() { return input_; }
	Renderer2D& GetRenderer() { return renderer_; }
	Renderer3D& GetRenderer3D() { return renderer3D_; }
	FrameTimer& GetTimer() { return timer_; }

	// 배경 색상 설정 (매 프레임 화면을 지울 때 쓰는 색. 모든 창에 공통 적용)
	void SetClearColor(const color& _color) { clearColor_ = _color; }
	const color& GetClearColor() const { return clearColor_; }

	// 수직동기화 설정 (true면 모니터 주사율에 맞춰 대기. 티어링 방지)
	void SetVsync(bool _vsync) { vsync_ = _vsync; }

protected:
	////////////////////////////////////////////////////////////////////////////////////////
	// Cocos2d-x AppDelegate 스타일 생명주기 훅
	// 초기화 완료 직후 1회. 보통 여기서 첫 씬을 시작한다.
	// false를 반환하면 초기화 실패로 처리된다.
	virtual bool ApplicationDidFinishLaunching() { return true; }

	// 종료 직전 1회.
	virtual void ApplicationDidExit() {}

	// 창이 비활성화(다른 창으로 전환)될 때. 기본 동작: 소리 전체 일시정지.
	virtual void ApplicationDidEnterBackground();

	// 창이 다시 활성화될 때. 기본 동작: 소리 전체 재개.
	virtual void ApplicationWillEnterForeground();

	// 매 프레임 훅. _dt는 jc::TimeSpan이다. (씬 갱신 후에 불린다)
	virtual void OnUpdate(const jc::TimeSpan& _dt) { (void)_dt; }

	// 매 프레임 그리기 훅. (메인 윈도우의 씬 그리기 후에 불린다)
	virtual void OnRender() {}

private:
	// 닫힌 서브 윈도우를 정리한다. (매 프레임 그리기 전에 호출)
	void DestroyClosedSubWindows();

private:
	static constexpr _s32 ACTIVATION_LISTENER_ID = 1;	// Window::onActivated 구독 ID

	Window window_;						// 메인 윈도우 (닫히면 앱 종료)
	jc::Vector<Window*> subWindows_;	// 서브 윈도우들 [소유)
	GraphicDevice device_;				// DX11 디바이스 파사드 (앱 전체 1개)
	InputManager input_;				// 키보드/마우스 입력 (메인 윈도우 전용)
	Renderer2D renderer_;				// 2D 배치 렌더러 (전역 1개)
	Renderer3D renderer3D_;				// 3D 배치 렌더러 (전역 1개,)
	FrameTimer timer_;					// jc::TimeSpan 기반 시간 측정
	color clearColor_;					// 배경 색상
	bool vsync_;						// 수직동기화 여부
	bool initialized_;					// 중복 초기화/종료 방지
};

// 현재 실행 중인 Application 인스턴스 (Initialize에서 설정된다)
// sgcl처럼 전역 어디서든 g_c 매크로로 서브시스템에 접근하기 위한 포인터다.
inline Application* __sSgfApplication = nullptr;

NS_SGF_END

////////////////////////////////////////////////////////////////////////////////////////////
// sgcl 스타일 전역 접근 매크로
// sgcl과 동일하게 g_c 접두사 + "레퍼런스" 접근 방식을 사용한다.
// 예) g_cWindow.Width(), g_cInput.IsKeyPressed(VK_SPACE), g_cRenderer3D.DrawCube(...)
#define g_cApp        (*sgf::__sSgfApplication)
#define g_cWindow     (sgf::__sSgfApplication->GetWindow())
#define g_cDevice     (sgf::__sSgfApplication->GetDevice())
#define g_cInput      (sgf::__sSgfApplication->GetInput())
#define g_cRenderer2D (sgf::__sSgfApplication->GetRenderer())
#define g_cRenderer3D (sgf::__sSgfApplication->GetRenderer3D())
#define g_cTimer      (sgf::__sSgfApplication->GetTimer())
