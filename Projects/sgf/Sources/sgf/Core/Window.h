/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:35:00 PM
 * 수정일: 8/9/2026 9:30:00 AM (v2.1: 멀티 윈도우 지원 - 윈도우가 자신의 그리기 표면을 소유)
 * =====================
 * Win32 윈도우 래퍼 클래스
 *
 * [윈도우가 만들어지는 과정]
 *  1. RegisterClassEx : "이런 종류의 창을 만들거야"라고 OS에 윈도우 클래스를 등록
 *  2. CreateWindowEx  : 등록한 클래스로 실제 창을 생성 (HWND 핸들을 받음)
 *  3. ShowWindow      : 창을 화면에 표시
 *  4. 메시지 루프      : OS가 보내는 이벤트(키보드/마우스/종료 등)를 계속 꺼내서 처리
 *
 * [메시지 루프란?]
 *  윈도우 프로그램은 "이벤트 기반"으로 동작한다.
 *  마우스를 움직이면 OS가 WM_MOUSEMOVE 메시지를 프로그램의 메시지 큐에 넣어주고,
 *  프로그램은 PeekMessage/GetMessage로 이를 꺼내 WndProc에서 처리한다.
 *  게임은 메시지가 없을 때도 계속 그려야 하므로, 블로킹되는 GetMessage 대신
 *  비블로킹 PeekMessage를 사용하는 것이 핵심 차이점이다.
 *
 * [v2.1에서 바뀐 점 - 윈도우가 그리기 표면을 소유]
 *  멀티 윈도우를 지원하려면 "백버퍼(그림이 그려지는 종이)"가 창마다 하나씩 필요하다.
 *  그래서 스왕체인(SwapChain)과 깊이버퍼(DepthStencilSurface)를
 *  GraphicDevice가 아닌 각 Window가 소유하도록 바꿨다.
 *  (디바이스/컨텍스트는 앱 전체에 1개, 표면은 창마다 1개씩 - 이게 자연스러운 소유 구조다)
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/SwapChain.h"
#include "sgf/Graphics/DepthStencilSurface.h"

NS_SGF_BEGIN

class InputManager;
class GraphicDevice;

class Window
{
public:
	Window();
	~Window();

	// 윈도우를 생성한다.
	// @param _title  : 창 제목표시줄에 보일 문자열
	// @param _width  : 클라이언트 영역(실제 그림이 그려지는 부분)의 가로 크기
	// @param _height : 클라이언트 영역의 세로 크기
	// @return 성공 여부
	bool Create(const wchar_t* _title, _s32 _width, _s32 _height);

	// 밀린 메시지를 모두 처리한다. 매 프레임 시작 시 호출해야 한다.
	// (PeekMessage는 "이 스레드의 모든 창" 메시지를 꺼내므로,
	//  메인 윈도우 하나만 호출해도 서브 윈도우 메시지까지 함께 처리된다)
	// @return 종료 메시지(WM_QUIT)를 받았으면 false (루프를 나가라는 의미)
	bool PumpMessage();

	// 윈도우를 파괴한다. (WM_DESTROY 유발)
	void Destroy();

	// 창 제목을 변경한다. (FPS 표시 등에 활용)
	void SetTitle(const wchar_t* _title);

	// 입력 메시지를 전달받을 InputManager를 연결한다.
	// 연결하면 WndProc에서 키/마우스 메시지를 InputManager에게 넘겨준다.
	void ConnectInput(InputManager* _pInput) { pInput_ = _pInput; }

	// === v2.1: 윈도우별 그리기 표면 (스왕체인 + 깊이버퍼) ===

	// 이 윈도우 전용 스왕체인/깊이버퍼를 생성한다. (창 생성 후 1회)
	// GraphicDevice::CreateSwapChainForWindow가 DXGI 팩토리로 스왕체인을 만들어준다.
	bool CreateSurface(GraphicDevice* _pDevice);

	// 그리기 표면을 해제한다. (창 파괴 전 호출)
	void DestroySurface();

	// 그리기 표면을 가지고 있는가? (CreateSurface 성공 여부)
	bool HasSurface() const { return hasSurface_; }

	SwapChain& GetSwapChain() { return swapChain_; }
	DepthStencilSurface& GetDepthSurface() { return depthSurface_; }

	HWND Handle() const { return hWnd_; }
	_s32 Width() const { return width_; }
	_s32 Height() const { return height_; }

	// 가로/세로 비율 (투영 행렬 계산에 필요)
	_f32 AspectRatio() const { return height_ > 0 ? _f32(width_) / _f32(height_) : 1.0f; }

	// 창이 닫혔는지 여부
	bool IsClosed() const { return closed_; }

	// 창 활성/비활성 이벤트 (true=활성화, false=비활성화)
	// Application이 구독해서 cocos 스타일 background/foreground 훅을 부른다.
	jc::Event<bool> onActivated;

private:
	// 윈도우 프로시저: OS가 보내는 메시지를 처리하는 콜백 함수
	// static인 이유: Win32 API는 C 스타일이라 멤버 함수 포인터를 받을 수 없다.
	// 대신 CreateWindowEx의 lpParam으로 this를 넘겨받아 GWLP_USERDATA에 보관해두고
	// 메시지가 올 때마다 인스턴스를 복원해서 멤버 함수로 위임한다.
	static LRESULT CALLBACK WndProcStatic(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);

	// 실제 메시지 처리 (멤버 함수 버전)
	LRESULT WndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);

private:
	HWND hWnd_;						// 윈도우 핸들 (OS가 발급한 창의 식별자)
	_s32 width_;						// 클라이언트 영역 가로
	_s32 height_;						// 클라이언트 영역 세로
	bool closed_;						// 창이 닫혔는지 여부
	bool hasSurface_;					// 그리기 표면 보유 여부
	InputManager* pInput_;				// 입력 메시지를 전달할 대상 (소유하지 않음)
	SwapChain swapChain_;				// 이 창 전용 백버퍼 교체 장치
	DepthStencilSurface depthSurface_;	// 이 창 전용 깊이/스텐실 버퍼
};

NS_SGF_END
