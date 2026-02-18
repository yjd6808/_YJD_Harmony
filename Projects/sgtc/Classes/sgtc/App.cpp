#include "Core.h"
#include "App.h"

#include <sgtc/_Scene/Scene_World.h>

#define APP_NAME "Test-Client"

USING_NS_CC;
USING_NS_JC;
USING_NS_JC_DETAIL;
USING_NS_JNET;
USING_NS_JNET_DETAIL;

//////////////////////////////////////////////////////////////////////////////////////////
App::App(int _argc, char** _argv)
: pWndProcHook_(nullptr)
, argc_(_argc)
, argv_(_argv)
{
	g_cApp; // 초기화
}

//////////////////////////////////////////////////////////////////////////////////////////
App::~App()
{
	// 여기서 코코스관련 오브젝트 삭제되도록 하면 릭 발생위험 있음.
	// 예를 들어서 Label의 경우 _fontAtlas 멤버 변수가 포함되어있는데
	// 라벨같은 코코스 오브젝트는 메인 함수의 Application->run() 함수 마무리단계에서 FontAtlasCache::purgeCachedData() 함수의 호출로 해제되도록 되어있다.
	// 근데 내가 코코스 게임엔진의 모든 리소스가 종료되고 나서도 코코스 오브젝트 레퍼런스를 유지하고 있어버리면
	// 일일히 찾아서 해제해줄 수가 없다.
	// 따라서 WorldScene 삭제시 해제해주도록 하자.
	if (pWndProcHook_ != nullptr)
	{
		UnhookWindowsHookEx(pWndProcHook_);
		pWndProcHook_ = nullptr;
	}

	__sApp = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void App::initGLContextAttrs()
{
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };
	GLView::setGLContextAttrs(glContextAttrs);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool App::applicationDidFinishLaunching()
{
	// ======================================================
	// 메인 리소스 초기화
	// ======================================================
	InitializeJCore(argc_, argv_);
	FileUtils::getInstance()->setPopupNotify(false); // 파일못찾은 경우 알람 안하도록 함
	jc::Console::SetSize(1200, 800);

	#define LOG_SPECIFIER_CLIENT	CSI VT_RESET CSI_GRAPHIC_RENDITION_END "( " CSI VT_FORE_COLOR_LIGHT_BLUE	CSI_GRAPHIC_RENDITION_END "★" CSI VT_RESET CSI_GRAPHIC_RENDITION_END " )"
	InitializeNetLogger(LOG_SPECIFIER_CLIENT);
	InitializeDefaultLogger(LOG_SPECIFIER_CLIENT);
	CreateOpenGLWindow(); // 윈도우 생성 후 클라이언트 코어 로딩
	CreateWorldScene();
	
	InitializeWindowProcedure();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void App::CreateOpenGLWindow()
{
	auto pDirector = Director::getInstance();
	auto pGlView = pDirector->getOpenGLView();
	Rect frameRect{ 0, 0, 1280.f, 720.f };

	bool isFullScreen = false;
	bool isResizable = true;
	Size resolutionSize{ frameRect.size };

	if (pGlView == nullptr)
	{
		pGlView = isFullScreen
			? GLViewImpl::createWithFullScreen(APP_NAME)
			: GLViewImpl::createWithRect(APP_NAME, frameRect, 1.0f, isResizable);
		pGlView->setDesignResolutionSize(
			resolutionSize.width,
			resolutionSize.height,
			ResolutionPolicy::NO_BORDER);
	}

	pDirector->setOpenGLView(pGlView);
	pDirector->setDisplayStats(false);
	pDirector->setAnimationInterval(1.0f / 120);
	pDirector->setContentScaleFactor(1.0f);

	pView_ = static_cast<GLViewImpl*>(pGlView);
}

//////////////////////////////////////////////////////////////////////////////////////////
void App::applicationDidExit(ExitStep _step)
{
	switch (_step)
	{
	case ExitStep::EXIT_GAME_LOOP:
		{
		}
		break;
	case ExitStep::EXIT_DIRECTOR: // 월드씬 정리 된 후 호출
		{
		}
		break;
	case ExitStep::EXIT_GLVIEW: // GLVIew까지 정리된 후 호출
		{


		}
		break;
	}

}


//////////////////////////////////////////////////////////////////////////////////////////
void App::CreateWorldScene()
{
	auto pScene = WorldScene::Get();
	pScene->setAnchorPoint(Vec2::ZERO);
	jc_assert_msg(pScene, "월드씬 생성에 실패했습니다.");
	Director::getInstance()->runWithScene(pScene);
}

//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK App::GLFWWindowHookProc(int _code, WPARAM _wParam, LPARAM _lParam)
{
	App* pApp = static_cast<App*>(Application::getInstance());
	Scene* pRunningScene = Director::getInstance()->getRunningScene();
	WorldScene* pWorld;

	if (pRunningScene != nullptr && pApp != nullptr && (pWorld = dynamic_cast<WorldScene*>(pRunningScene)) != nullptr)
	{
	}

	_LogDebug_("%d", _code);
	return ::CallNextHookEx(NULL, _code, _wParam, _lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK App::GLFWWindowProc(HWND _pHwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
{
	App* pApp = static_cast<App*>(Application::getInstance());
	Scene* pRunningScene = Director::getInstance()->getRunningScene();
	WorldScene* pWorld = nullptr;

	return ::CallWindowProcW(pApp->GetPrevWndProc(), _pHwnd, _uMsg, _wParam, _lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////
void App::InitializeWindowProcedure()
{
	return;

	const HWND pWndCocos = Director::getInstance()->getOpenGLView()->getWin32Window();
	// TODO: SetWindowsHookExA 함수로 후킹하면 메모리릭이 대량 발생하는데.. 원인을 잘 모르겠다.
	/*
	 const DWORD dwThreadId = GetWindowThreadProcessId(pWndCocos, NULL);
	 const HINSTANCE hInstance = ::GetModuleHandleW(nullptr);
	 pWndProcHook_ = SetWindowsHookExA(WH_CALLWNDPROC, GLFWWindowHookProc, hInstance, dwThreadId);
	 if (pWndProcHook_ == nullptr)
		 _LogError_("코코스 윈도우 프로시저 후킹에 실패했습니다.");
	*/

	// TODO: SetWindowLongPtrW로 프로시저 처리해도 릭 발생함. 원인을 잘 모르겠다. (동일한 원인 같은데)
	// pPrevWndProc_ = (WNDPROC)SetWindowLongPtrW(pWndCocos, GWLP_WNDPROC, (LONG_PTR)GLFWWindowProc);
}

//////////////////////////////////////////////////////////////////////////////////////////
void App::applicationDidEnterBackground()
{
	Director::getInstance()->stopAnimation();
}

//////////////////////////////////////////////////////////////////////////////////////////
void App::applicationWillEnterForeground()
{
	Director::getInstance()->startAnimation();
}