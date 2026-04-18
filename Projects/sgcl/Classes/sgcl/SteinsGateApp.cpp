#include "SteinsGateApp.h"
#include "GameCoreHeader.h"

#include "jc/Logger/ConsoleLogger.h"

#include "sg/Struct/SteinsGate_Client.h"

#include "sg/LogSpecifier.h"
#include "sg/Core/AudioPlayer.h"
#include "sg/Sga/SgaElementInitializer.h"
#include "sg/Core/AppConfig.h"

#include "sgcl/Util/Win32Helper.h"
#include "sgcl/Scene/Scene_World.h"

#include "API/sgapiClient.h"

#define APP_NAME "SteinsGate-Client"

USING_NS_CC;
USING_NS_JC;
USING_NS_JC_DETAIL;
USING_NS_JNET;
USING_NS_JNET_DETAIL;

//////////////////////////////////////////////////////////////////////////////////////////
SteinsGateApp::SteinsGateApp(int _argc, char** _argv)
: pWndProcHook_(nullptr)
, argc_(_argc)
, argv_(_argv)
{
	g_cApp; // 초기화
}

//////////////////////////////////////////////////////////////////////////////////////////
SteinsGateApp::~SteinsGateApp()
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

	SgaElementInitializer::Finalize();
	AudioPlayer::Finalize();

	__sSteinsGateApp = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SteinsGateApp::SetDesignResolutionSize(float _width, float _height)
{
	auto pDirector = Director::getInstance();
	auto pGlView = pDirector->getOpenGLView();

	jc_assert_msg(pView_, "아직 View가 설정되지 않았습니다.");
	pView_->setDesignResolutionSize(_width, _height, pGlView->getResolutionPolicy());
}

//////////////////////////////////////////////////////////////////////////////////////////
cc::rect SteinsGateApp::GetDesignResolutionRect() const
{
	cc::size size = pView_->getDesignResolutionSize();
	return cc::rect{ 0, 0, size.width, size.height };
}

//////////////////////////////////////////////////////////////////////////////////////////
cc::size SteinsGateApp::GetUIResolutionSize() const
{
	ClientInfo* pInfo = g_cAppConfig.GetClientInfo();
	return cc::size{ pInfo->uiResolutionWidth_, pInfo->uiResolutionHeight_ };
}

//////////////////////////////////////////////////////////////////////////////////////////
cc::vec2 SteinsGateApp::GetUIScaleFactor() const
{
	ClientInfo* pInfo = g_cAppConfig.GetClientInfo();
	cc::size designSize = pView_->getDesignResolutionSize();
	return cc::vec2{ designSize.width / pInfo->uiResolutionWidth_, designSize.height / pInfo->uiResolutionHeight_ };
}

//////////////////////////////////////////////////////////////////////////////////////////
float SteinsGateApp::GetUIScaleXFactor() const
{
	ClientInfo* pInfo = g_cAppConfig.GetClientInfo();
	return pView_->getDesignResolutionSize().width / pInfo->uiResolutionWidth_;
}

//////////////////////////////////////////////////////////////////////////////////////////
float SteinsGateApp::GetUIScaleYFactor() const
{
	ClientInfo* pInfo = g_cAppConfig.GetClientInfo();
	return pView_->getDesignResolutionSize().height / pInfo->uiResolutionHeight_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SteinsGateApp::SetFrameSize(float _width, float _height)
{
	jc_assert_msg(pView_, "아직 View가 설정되지 않았습니다.");
	pView_->setFrameSize(_width, _height);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SteinsGateApp::initGLContextAttrs()
{
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };
	GLView::setGLContextAttrs(glContextAttrs);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SteinsGateApp::applicationDidFinishLaunching()
{
	// ======================================================
	// 메인 리소스 초기화
	// ======================================================

	try
	{
		InitializeJCore(argc_, argv_);
		sgapiBase::Init(dbg_new sgapiClient);
		Winsock::Initialize(2, 2);
		AudioPlayer::Initilize();
		SgaElementInitializer::Initialize();
		FileUtils::getInstance()->setPopupNotify(false); // 파일못찾은 경우 알람 안하도록 함
		jc::Console::SetSize(1200, 800);

		InitializeNetLogger(LOG_SPECIFIER_CLIENT);
		InitializeDefaultLogger(LOG_SPECIFIER_CLIENT);
		InitializeCommonCore();

		CreateOpenGLWindow(); // 윈도우 생성 후 클라이언트 코어 로딩
		InitializeClientCore();
		InitializeClientLogo(true, 5);

		sg::Contents.Initialize();
		CreateWorldScene();

		InitializeWindowProcedure();
	}
	catch (std::exception& exception)
	{
		_LogError_("애플리케이션 초기화 중 오류가 발생하였습니다.\n%s", exception.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SteinsGateApp::CreateOpenGLWindow()
{
	ClientInfo* pClientInfo = g_cAppConfig.GetClientInfo();
	jc_assert(pClientInfo);

	auto pDirector = Director::getInstance();
	auto pGlView = pDirector->getOpenGLView();
	Rect frameRect{ 0, 0, pClientInfo->frameWidth_, pClientInfo->frameHeight_ };

	if (pGlView == nullptr)
	{
		pGlView = pClientInfo->fullScreen_
			? GLViewImpl::createWithFullScreen(APP_NAME)
			: GLViewImpl::createWithRect(APP_NAME, frameRect, 1.0f, pClientInfo->resizable_);
		pGlView->setDesignResolutionSize(
			pClientInfo->gameResolutionWidth_,
			pClientInfo->gameResolutionHeight_,
			(ResolutionPolicy)pClientInfo->gameResolutionPolicy_);
	}

	pDirector->setOpenGLView(pGlView);
	pDirector->setDisplayStats(false);
	pDirector->setAnimationInterval(1.0f / 120);
	pDirector->setContentScaleFactor(1.0f);

	Win32Helper::LazyInit();

	pView_ = static_cast<GLViewImpl*>(pGlView);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SteinsGateApp::applicationDidExit(ExitStep _step)
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
void SteinsGateApp::CreateWorldScene()
{
	auto pScene = WorldScene::Get();
	pScene->setAnchorPoint(Vec2::ZERO);
	jc_assert_msg(pScene, "월드씬 생성에 실패했습니다.");
	Director::getInstance()->runWithScene(pScene);
}

//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK SteinsGateApp::GLFWWindowHookProc(int _code, WPARAM _wParam, LPARAM _lParam)
{
	SteinsGateApp* pApp = static_cast<SteinsGateApp*>(Application::getInstance());
	Scene* pRunningScene = Director::getInstance()->getRunningScene();
	WorldScene* pWorld;

	if (pRunningScene != nullptr && pApp != nullptr && (pWorld = dynamic_cast<WorldScene*>(pRunningScene)) != nullptr)
	{
	}

	_LogDebug_("%d", _code);
	return ::CallNextHookEx(NULL, _code, _wParam, _lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK SteinsGateApp::GLFWWindowProc(HWND _pHwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
{
	SteinsGateApp* pApp = static_cast<SteinsGateApp*>(Application::getInstance());
	Scene* pRunningScene = Director::getInstance()->getRunningScene();
	WorldScene* pWorld = nullptr;

	return ::CallWindowProcW(pApp->GetPrevWndProc(), _pHwnd, _uMsg, _wParam, _lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SteinsGateApp::InitializeWindowProcedure()
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
void SteinsGateApp::applicationDidEnterBackground()
{
	Director::getInstance()->stopAnimation();
}

//////////////////////////////////////////////////////////////////////////////////////////
void SteinsGateApp::applicationWillEnterForeground()
{
	Director::getInstance()->startAnimation();
}
