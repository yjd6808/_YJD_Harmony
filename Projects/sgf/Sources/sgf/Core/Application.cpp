/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:44:00 AM
 * 수정일: 8/9/2026 9:30:00 AM (v2.1: 멀티 윈도우 + Renderer3D + 창 표면 분리)
 * =====================
 * 애플리케이션 구현부 (게임 루프)
 */

#include "Core.h"
#include "sgf/Core/Application.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgf/Scene/Director.h"
#include "sgf/Audio/SoundEngine.h"

NS_SGF_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자: 기본값 설정 (진한 회색 배경, 수직동기화 켜짐)
Application::Application()
	: clearColor_(0.1f, 0.1f, 0.1f, 1.0f)
	, vsync_(true)
	, initialized_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 소멸자: Finalize를 깜빡해도 안전하도록 보호장치
Application::~Application()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 엔진 초기화 (v2.1)
// [초기화 순서가 중요한 이유]
//  1. 윈도우 생성      : 스왑체인이 붙을 HWND가 필요하다.
//  2. 디바이스 초기화  : "그림 그리는 공장"을 먼저 세운다. (창과 무관, 앱 전체 1개)
//  3. 창 표면 생성     : 공장에서 이 창 전용 스왑체인/깊이버퍼를 만들어 창에 붙인다.
//  4. 렌더러 초기화    : 셰이더/버퍼 생성에 디바이스를 쓴다. (2D/3D 각각 전역 1개)
//  즉, 뒤의 단계는 앞 단계의 결과물에 의존한다.
bool Application::Initialize(const wchar_t* _szTitle, _s32 _width, _s32 _height)
{
	if (initialized_)
	{
		return true;
	}

	// 0. g_c 매크로가 동작하도록 전역 포인터를 가장 먼저 연결한다.
	__sSgfApplication = this;

	// 1. 메인 윈도우 생성 + 입력 관리자 연결
	if (!window_.Create(_szTitle, _width, _height))
	{
		OutputDebugStringA("[sgf] 윈도우 생성 실패\n");
		__sSgfApplication = nullptr;
		return false;
	}
	window_.ConnectInput(&input_);

	// 2. DX11 디바이스 초기화 (v2.1: 창과 분리된 "디바이스만" 만든다)
	if (!device_.Initialize())
	{
		OutputDebugStringA("[sgf] 그래픽 디바이스 초기화 실패\n");
		__sSgfApplication = nullptr;
		return false;
	}

	// 2.5. 리소스 매니저 초기화 (v3: 디폴트 셰이더/머티리얼/흰색 텍스처 준비)
	if (!g_cResourceMgr.Initialize(&device_))
	{
		OutputDebugStringA("[sgf] 리소스 매니저 초기화 실패\n");
		__sSgfApplication = nullptr;
		return false;
	}

	// 3. 메인 윈도우에 그리기 표면(스왑체인 + 깊이버퍼)을 붙인다. (v2.1)
	if (!window_.CreateSurface(&device_))
	{
		OutputDebugStringA("[sgf] 메인 윈도우 표면 생성 실패\n");
		__sSgfApplication = nullptr;
		return false;
	}

	// 4. 2D 렌더러 초기화 (셰이더 컴파일, 배치 버퍼 생성)
	if (!renderer_.Initialize(&device_))
	{
		OutputDebugStringA("[sgf] 2D 렌더러 초기화 실패\n");
		__sSgfApplication = nullptr;
		return false;
	}

	// 5. 3D 렌더러 초기화 (v2.1: 2D와 동급인 전역 배치 렌더러)
	if (!renderer3D_.Initialize(&device_))
	{
		OutputDebugStringA("[sgf] 3D 렌더러 초기화 실패\n");
		__sSgfApplication = nullptr;
		return false;
	}

	// 6. 사운드 엔진 초기화 (실패해도 게임은 계속 - 소리만 안 날 뿐)
	if (!SoundEngine::Get()->Initialize())
	{
		OutputDebugStringA("[sgf] 사운드 엔진 초기화 실패 (소리 없이 계속 진행)\n");
	}

	// 7. 창 활성/비활성 이벤트 구독
	//    (Cocos2d-x의 applicationDidEnterBackground/WillEnterForeground 대응)
	window_.onActivated.Register(ACTIVATION_LISTENER_ID, [this](bool _active)
	{
		if (_active)
		{
			ApplicationWillEnterForeground();
		}
		else
		{
			ApplicationDidEnterBackground();
		}
	});

	// 8. 타이머 시작 (이 순간부터 dt 측정)
	timer_.Reset();
	initialized_ = true;

	// 9. 파생 앱 훅: 보통 여기서 첫 씬을 시작한다. (cocos의 AppDelegate와 동일)
	if (!ApplicationDidFinishLaunching())
	{
		OutputDebugStringA("[sgf] ApplicationDidFinishLaunching 실패\n");
		Finalize();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// [v2.1] 서브 윈도우 생성
// 애플리케이션은 창을 여러 개 가질 수 있다. 각 창은 자기만의
// 표면(스왑체인 + 깊이버퍼)을 갖고, 디바이스(공장)는 모두가 공유한다.
// 반환된 창에 g_cDirector.RunScene(pScene, pWindow)로 씬을 올리면 된다.
Window* Application::CreateSubWindow(const wchar_t* _szTitle, _s32 _width, _s32 _height)
{
	jc_assert_msg(initialized_, "Initialize 이후에만 서브 윈도우를 만들 수 있습니다");

	Window* pWindow = new Window();
	if (!pWindow->Create(_szTitle, _width, _height))
	{
		OutputDebugStringA("[sgf] 서브 윈도우 생성 실패\n");
		JC_DELETE_SAFE(pWindow);
		return nullptr;
	}

	// 서브 창이 닫혀도 프로그램 전체가 종료되지 않는다.
	// (창 파괴는 PumpMessage가 창 인스턴스별 closed_로 감지하므로
	//  서브 윈도우의 파괴가 메인 루프 종료로 이어지지 않는다)
	if (!pWindow->CreateSurface(&device_))
	{
		OutputDebugStringA("[sgf] 서브 윈도우 표면 생성 실패\n");
		pWindow->Destroy();
		JC_DELETE_SAFE(pWindow);
		return nullptr;
	}

	subWindows_.PushBack(pWindow);
	return pWindow;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 게임 루프
// [한 프레임의 흐름 - v2.1 멀티 윈도우]
//  메시지 펌프 -> 시간 측정 -> 씬/앱 갱신 -> 닫힌 창 정리
//  -> 창마다 [BeginFrame -> 씬 그리기 -> EndFrame]
//  -> 입력 프레임 넘김 -> 사운드 정리
void Application::Run()
{
	if (!initialized_)
	{
		return;
	}

	while (true)
	{
		// 1. 윈도우 메시지 처리. WM_QUIT이면 루프 종료.
		//    (PeekMessage가 스레드의 모든 창 메시지를 함께 처리하므로
		//     서브 윈도우는 따로 펌프할 필요가 없다)
		if (!window_.PumpMessage())
		{
			break;
		}

		// 2. 직전 프레임으로부터 경과 시간 측정 (v2: jc::TimeSpan)
		timer_.Tick();
		const jc::TimeSpan& dt = timer_.Delta();

		// 3. 로직 갱신: 모든 윈도우의 씬 교체/갱신 -> 앱 훅
		g_cDirector.Update(dt);
		OnUpdate(dt);

		// 3.5. OnUpdate(ESC 종료 등)에서 메인 윈도우가 닫혔으면 즉시 루프 종료.
		//      이 체크가 없으면 파괴된 창(HasSurface=false)으로 BeginFrame을
		//      시도해 assert가 발생한다.
		if (window_.IsClosed())
		{
			break;
		}

		// 4. 사용자가 닫은 서브 윈도우 정리 (그리기 전에!)
		DestroyClosedSubWindows();

		// 5. 메인 윈도우 그리기: 화면 지우기 -> 씬 그리기 -> 앱 훅 -> 화면 표시
		device_.BeginFrame(&window_, clearColor_);
		g_cDirector.Render(&window_);
		OnRender();
		device_.EndFrame(&window_, vsync_);

		// 6. 서브 윈도우들 그리기 (v2.1)
		for (_s32 i = 0; i < subWindows_.Size(); ++i)
		{
			Window* pSub = subWindows_[i];
			if (pSub->IsClosed() || !pSub->HasSurface())
			{
				continue;
			}
			device_.BeginFrame(pSub, clearColor_);
			g_cDirector.Render(pSub);
			device_.EndFrame(pSub, vsync_);
		}

		// 7. 입력 상태를 다음 프레임으로 넘긴다.
		//    (IsKeyPressed 같은 "막 눌림" 판정이 이 호출에 의존한다)
		input_.NextFrame();

		// 8. 사운드 엔진 정리 (재생이 끝난 소리의 재생기 회수)
		g_cSound.Update();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// [v2.1] 닫힌 서브 윈도우 정리
// 사용자가 X 버튼으로 닫은 창의 씬/표면/메모리를 회수한다.
void Application::DestroyClosedSubWindows()
{
	for (_s32 i = subWindows_.Size() - 1; i >= 0; --i)
	{
		Window* pSub = subWindows_[i];
		if (!pSub->IsClosed())
		{
			continue;
		}

		// 1) 이 창에 올라간 씬을 내리고 (OnExit + delete)
		g_cDirector.DetachWindow(pSub);

		// 2) 창 표면과 창 자체를 파괴한다.
		pSub->DestroySurface();
		pSub->Destroy();
		JC_DELETE_SAFE(pSub);

		// 3) 목록에서 제거 (뒤에서부터 밀어 넣기)
		for (_s32 k = i; k < subWindows_.Size() - 1; ++k)
		{
			subWindows_[k] = subWindows_[k + 1];
		}
		subWindows_.Resize(subWindows_.Size() - 1);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 엔진 종료: 초기화의 역순으로 정리한다.
void Application::Finalize()
{
	if (!initialized_)
	{
		return;
	}
	initialized_ = false;

	ApplicationDidExit();			// 파생 앱 정리 훅
	g_cDirector.Cleanup();			// 모든 윈도우의 씬 정리 (텍스처 등 리소스 반납)

	// 서브 윈도우 정리 (v2.1)
	for (_s32 i = 0; i < subWindows_.Size(); ++i)
	{
		Window* pSub = subWindows_[i];
		pSub->DestroySurface();
		pSub->Destroy();
		JC_DELETE_SAFE(pSub);
	}
	subWindows_.Clear();

	g_cSound.Finalize();			// 사운드 엔진 정리 (XAudio2 해제)
	renderer3D_.Finalize();		// 3D 렌더러 리소스 해제 (v2.1)
	renderer_.Finalize();			// 2D 렌더러 리소스 해제
	g_cResourceMgr.Finalize();		// 리소스 매니저 정리 (v3: 디폴트 리소스 포함 소멸)
	window_.DestroySurface();		// 메인 창 표면 해제 (디바이스보다 먼저)
	device_.Finalize();			// DX11 디바이스 해제
	window_.Destroy();				// 메인 윈도우 파괴

	__sSgfApplication = nullptr;	// 전역 포인터 해제
}

//////////////////////////////////////////////////////////////////////////////////////////
// 기본 동작: 백그라운드로 가면 모든 소리를 일시정지한다.
// (Cocos2d-x AppDelegate::applicationDidEnterBackground의 AudioEngine::pauseAll과 동일)
void Application::ApplicationDidEnterBackground()
{
	g_cSound.PauseAll();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 기본 동작: 다시 활성화되면 일시정지했던 소리를 재개한다.
void Application::ApplicationWillEnterForeground()
{
	g_cSound.ResumeAll();
}

NS_SGF_END
