#include "Tutturu.h"
#include "SteinsGateApp.h"
#include "GameCoreHeader.h"

#include <JCore/Logger/ConsoleLogger.h>

#include <SteinsGate/Common/LogSpecifier.h>
#include <SteinsGate/Common/AudioPlayer.h>
#include <SteinsGate/Common/SgaElementInitializer.h>
#include <SteinsGate/Client/Win32Helper.h>

#define AppName "SteinsGate-Client"

USING_NS_CC;
USING_NS_JC;
USING_NS_JC_DETAIL;
USING_NS_JNET;
USING_NS_JNET_DETAIL;

SteinsGateApp::SteinsGateApp()
	: m_hWndProcHook(nullptr)
{}

SteinsGateApp::~SteinsGateApp() 
{
    // 여기서 코코스관련 오브젝트 삭제되도록 하면 릭 발생위험 있음.
    // 예를 들어서 Label의 경우 _fontAtlas 멤버 변수가 포함되어있는데
    // 라벨같은 코코스 오브젝트는 메인 함수의 Application->run() 함수 마무리단계에서 FontAtlasCache::purgeCachedData() 함수의 호출로 해제되도록 되어있다.
    // 근데 내가 코코스 게임엔진의 모든 리소스가 종료되고 나서도 코코스 오브젝트 레퍼런스를 유지하고 있어버리면
    // 일일히 찾아서 해제해줄 수가 없다.
    // 따라서 WorldScene 삭제시 해제해주도록 하자.
    if (m_hWndProcHook != nullptr)
		UnhookWindowsHookEx(m_hWndProcHook);

    SgaElementInitializer::Finalize();
    AudioPlayer::Finalize();
}

void SteinsGateApp::SetDesignResolutionSize(float width, float height) {
    DebugAssertMsg(View, "아직 View가 설정되지 않았습니다.");
    View->setDesignResolutionSize(width, height, Core::ClientInfo->GameResolutionPolicy);
}

void SteinsGateApp::SetFrameSize(float width, float height) {
    DebugAssertMsg(View, "아직 View가 설정되지 않았습니다.");
    View->setFrameSize(width, height);
}

void SteinsGateApp::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}



bool SteinsGateApp::applicationDidFinishLaunching() {

    // ======================================================
	// 메인 리소스 초기화
	// ======================================================

    InitializeJCore();
    Winsock::Initialize(2, 2);
    AudioPlayer::Initilize();
    SgaElementInitializer::Initialize();
    FileUtils::getInstance()->setPopupNotify(false);    // 파일못찾은 경우 알람 안하도록 함
    JCore::Console::SetSize(1200, 800);

    DataManager* pDataManager = DataManager::Get();
    pDataManager->initializeLoader();
    Core::CommonInfo = pDataManager->getCommonInfo(1);
    Core::ClientInfo = pDataManager->getClientInfo(1);
    Core::CharCommon = pDataManager->getCharCommonInfo(1);

    CreateOpenGLWindow();
    InitializeNetLogger(LOG_SPECIFIER_CLIENT);
    InitializeDefaultLogger(LOG_SPECIFIER_CLIENT);
    InitializeCommonCore();
    InitializeClientCore();
    InitializeClientLogo(true, 5);
    CreateWorldScene();
    InitializeWindowProcedure();
    
    return true;
}

void SteinsGateApp::CreateOpenGLWindow() {

    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    Rect resolutionRect = Core::ClientInfo->getGameResolutionRect();
    Rect frameRect = Core::ClientInfo->getFrameRect();

    if (glview == nullptr) {
        glview = Core::ClientInfo->FullScreen ? 
            GLViewImpl::createWithFullScreen(AppName) :
            GLViewImpl::createWithRect(AppName, frameRect, 1.0f, Core::ClientInfo->Resizable);
        glview->setDesignResolutionSize(resolutionRect.size.width, resolutionRect.size.height, Core::ClientInfo->GameResolutionPolicy);
    }

    director->setOpenGLView(glview);
	director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 120);
    director->setContentScaleFactor(1.0f);

    Win32Helper::LazyInit();

    View = (GLViewImpl*)glview;
}

void SteinsGateApp::CreateWorldScene() {
    auto scene = WorldScene::get();
    scene->setAnchorPoint(Vec2::ZERO);
    DebugAssertMsg(scene, "월드씬 생성에 실패했습니다.");
    Director::getInstance()->runWithScene(scene);
}

LRESULT CALLBACK SteinsGateApp::GLFWWindowHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    SteinsGateApp* pApp = (SteinsGateApp*)Application::getInstance();
    Scene* pRunningScene = Director::getInstance()->getRunningScene();
    WorldScene* pWorld;

    if (pRunningScene != nullptr && pApp != nullptr && (pWorld = dynamic_cast<WorldScene*>(pRunningScene)) != nullptr) {
        
    }

    _LogDebug_("%d", code);
    return ::CallNextHookEx(NULL, code, wParam, lParam);
}


LRESULT CALLBACK SteinsGateApp::GLFWWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    SteinsGateApp* pApp = (SteinsGateApp*)Application::getInstance();
    Scene* pRunningScene = Director::getInstance()->getRunningScene();
    WorldScene* pWorld;


    return ::CallWindowProcW(pApp->GetPrevWndProc(), hwnd, uMsg, wParam, lParam);
}


void SteinsGateApp::InitializeWindowProcedure() {
    return;

    const HWND hWndCocos = Director::getInstance()->getOpenGLView()->getWin32Window();
    // TODO: SetWindowsHookExA 함수로 후킹하면 메모리릭이 대량 발생하는데.. 원인을 잘 모르겠다.
    /*
     const DWORD dwThreadId = GetWindowThreadProcessId(hWndCocos, NULL);
     const HINSTANCE hInstance = ::GetModuleHandleW(nullptr);
     m_hWndProcHook = SetWindowsHookExA(WH_CALLWNDPROC, GLFWWindowHookProc, hInstance, dwThreadId);
	 if (m_hWndProcHook == nullptr)
	     _LogError_("코코스 윈도우 프로시저 후킹에 실패했습니다.");
	*/

    // TODO: SetWindowLongPtrW로 프로시저 처리해도 릭 발생함. 원인을 잘 모르겠다. (동일한 원인 같은데)
    // m_hPrevWndProc = (WNDPROC)SetWindowLongPtrW(hWndCocos, GWLP_WNDPROC, (LONG_PTR)GLFWWindowProc);
}

void SteinsGateApp::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void SteinsGateApp::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}

void SteinsGateApp::applicationDidExit() {

    
}

