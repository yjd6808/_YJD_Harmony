/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:36:00 PM
 * =====================
 * Win32 윈도우 래퍼 구현부
 */

#include "Core.h"
#include "sgf/Core/Window.h"
#include "sgf/Input/InputManager.h"
#include "sgf/Graphics/GraphicDevice.h"

NS_SGF_BEGIN

// 윈도우 클래스 이름 (RegisterClassEx에 등록할 식별자)
static const wchar_t* s_szWindowClassName = L"SgfWindowClass";

// 생성자: 멤버를 안전한 기본값으로 초기화한다.
Window::Window()
	: m_hWnd(nullptr)
	, m_Width(0)
	, m_Height(0)
	, m_bClosed(false)
	, m_bQuitOnClose(true)		// 기본은 메인 윈도우 취급 (닫히면 앱 종료)
	, m_bHasSurface(false)
	, m_pInput(nullptr)
{
}

// 소멸자: 생성된 윈도우가 남아있다면 파괴한다.
Window::~Window()
{
	Destroy();
}

// 윈도우를 생성한다.
// [진행 순서]
//  1. 윈도우 클래스 등록 (처음 한 번만)
//  2. 클라이언트 영역 크기 -> 실제 창 크기 보정 (AdjustWindowRect)
//  3. CreateWindowEx로 창 생성
//  4. ShowWindow로 화면에 표시
bool Window::Create(const wchar_t* _title, _s32 _width, _s32 _height)
{
	const HINSTANCE hInstance = GetModuleHandle(nullptr);

	// 1. 윈도우 클래스 등록
	// 이미 등록되어 있으면 RegisterClassEx가 실패하지만 무시해도 된다.
	// (튜토리얼을 여러 번 실행해도 문제없도록)
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;				// 크기 변경 시 전체 다시 그리기
	wc.lpfnWndProc = WndProcStatic;					// 메시지 처리 콜백 지정
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);	// 기본 화살표 커서
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = s_szWindowClassName;
	RegisterClassExW(&wc);

	// 2. 원하는 "클라이언트 영역" 크기를 실제 창 크기로 보정
	// 창에는 제목표시줄/테두리가 포함되므로 그만큼 더 크게 만들어야
	// 그림 그려지는 영역이 정확히 _width x _height가 된다.
	RECT rc = { 0, 0, _width, _height };
	const DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;	// 크기 고정 창
	AdjustWindowRect(&rc, dwStyle, FALSE);

	// 3. 창 생성
	// 마지막 인자로 this를 넘긴다. WM_NCCREATE에서 이 값을 꺼내
	// GWLP_USERDATA에 보관해두면 static 콜백에서 인스턴스를 찾을 수 있다.
	m_hWnd = CreateWindowExW(
		0,							// 확장 스타일 없음
		s_szWindowClassName,		// 등록한 클래스 이름
		_title,						// 창 제목
		dwStyle,					// 창 스타일
		CW_USEDEFAULT,				// x 위치 (OS에게 맡김)
		CW_USEDEFAULT,				// y 위치
		rc.right - rc.left,			// 보정된 창 가로 크기
		rc.bottom - rc.top,			// 보정된 창 세로 크기
		nullptr,					// 부모 창 없음
		nullptr,					// 메뉴 없음
		hInstance,
		this);						// WndProcStatic에서 사용할 사용자 데이터

	if (m_hWnd == nullptr)
	{
		return false;
	}

	m_Width = _width;
	m_Height = _height;
	m_bClosed = false;

	// 4. 창 표시
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	return true;
}

// 밀린 메시지를 모두 처리한다.
// PeekMessage는 메시지가 없으면 즉시 false를 반환하므로(비블로킹)
// 메시지가 없는 동안에는 게임 로직/렌더링을 수행할 수 있다.
// @return WM_QUIT을 받았으면 false (앱 종료)
bool Window::PumpMessage()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			m_bClosed = true;
			return false;
		}
		TranslateMessage(&msg);		// 가상 키 메시지를 문자 메시지(WM_CHAR)로 변환
		DispatchMessage(&msg);		// WndProc으로 메시지 전달
	}
	return true;
}

// 윈도우를 파괴한다.
void Window::Destroy()
{
	// 표면을 먼저 해제해야 한다. (스왕체인이 HWND를 참조하므로)
	DestroySurface();

	if (m_hWnd != nullptr)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}

	// 직접 파괴하는 경로(서브 윈도우 정리 등)에서도 닫힘 상태를 일관성 있게 유지한다.
	m_bClosed = true;
}

// 이 윈도우 전용 그리기 표면(스왕체인 + 깊이버퍼)을 생성한다.
// [왜 윈도우가 표면을 소유하는가?]
//  백버퍼는 "이 창에 보여질 그림"이므로 수명과 크기가 창에 종속된다.
//  창이 생기면 표면도 생기고, 창이 닫히면 표면도 사라지는 게 자연스럽다.
bool Window::CreateSurface(GraphicDevice* _pDevice)
{
	if (m_hWnd == nullptr || _pDevice == nullptr)
	{
		return false;
	}
	if (m_bHasSurface)
	{
		return true;	// 이미 생성됨 (중복 호출 무시)
	}

	// 1. 디바이스의 DXGI 팩토리로 이 창용 스왕체인을 만든다.
	IDXGISwapChain* pRawSwapChain = nullptr;
	if (!_pDevice->CreateSwapChainForWindow(m_hWnd, m_Width, m_Height, &pRawSwapChain))
	{
		return false;
	}

	// 2. 소유권을 SwapChain 부품에 넘기고 백버퍼 RTV를 만든다.
	if (!m_SwapChain.Initialize(pRawSwapChain, _pDevice->Device(), m_Width, m_Height))
	{
		return false;
	}

	// 3. 같은 크기의 깊이/스텐실 버퍼를 만든다.
	if (!m_DepthSurface.Initialize(_pDevice->Device(), m_Width, m_Height))
	{
		m_SwapChain.Finalize();
		return false;
	}

	m_bHasSurface = true;
	return true;
}

// 그리기 표면을 해제한다.
void Window::DestroySurface()
{
	if (!m_bHasSurface)
	{
		return;
	}
	m_DepthSurface.Finalize();
	m_SwapChain.Finalize();
	m_bHasSurface = false;
}

// 창 제목을 변경한다.
void Window::SetTitle(const wchar_t* _title)
{
	if (m_hWnd != nullptr)
	{
		SetWindowTextW(m_hWnd, _title);
	}
}

// static 메시지 콜백: 인스턴스를 복원해서 멤버 함수로 위임한다.
LRESULT CALLBACK Window::WndProcStatic(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	Window* pWindow = nullptr;

	if (_msg == WM_NCCREATE)
	{
		// CreateWindowEx의 마지막 인자(this)가 CREATESTRUCT에 담겨서 온다.
		// 이때 GWLP_USERDATA에 저장해두면 이후 메시지에서 꺼내 쓸 수 있다.
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(_lParam);
		pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
	}
	else
	{
		pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(_hWnd, GWLP_USERDATA));
	}

	if (pWindow != nullptr)
	{
		return pWindow->WndProc(_hWnd, _msg, _wParam, _lParam);
	}

	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}

// 실제 메시지 처리
// 입력 관련 메시지는 InputManager에게 위임하고,
// 창 관리 메시지(파괴 등)만 직접 처리한다.
LRESULT Window::WndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	// 입력 매니저가 연결되어 있으면 먼저 입력 메시지를 처리시킨다.
	if (m_pInput != nullptr)
	{
		m_pInput->HandleMessage(_msg, _wParam, _lParam);
	}

	switch (_msg)
	{
	case WM_DESTROY:
		// 창이 파괴되었다. 메인 윈도우(m_bQuitOnClose=true)라면
		// 메시지 루프를 끝내라는 WM_QUIT을 큐에 넣는다.
		// 서브 윈도우는 자신만 닫히고 앱은 계속 돌아간다. (v2.1 멀티 윈도우)
		m_bClosed = true;
		if (m_bQuitOnClose)
		{
			PostQuitMessage(0);
		}
		return 0;

	case WM_ACTIVATEAPP:
		// 창이 활성화/비활성화될 때 OS가 보내는 메시지.
		// wParam이 TRUE면 활성화, FALSE면 비활성화다.
		// 구독자(Application)가 있으면 사운드 일시정지/재개 등을 처리한다.
		onActivated.Invoke(_wParam != FALSE);
		break;

	case WM_KEYDOWN:
		// ESC 키로 창을 닫을 수 있게 한다. (튜토리얼 편의 기능)
		if (_wParam == VK_ESCAPE)
		{
			DestroyWindow(_hWnd);
		}
		return 0;

	default:
		break;
	}

	// 직접 처리하지 않는 메시지는 OS 기본 처리기에게 넘긴다.
	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}

NS_SGF_END
