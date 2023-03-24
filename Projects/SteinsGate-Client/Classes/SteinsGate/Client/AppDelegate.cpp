#include "Tutturu.h"
#include "AppDelegate.h"
#include "GameCoreHeader.h"
#include "SGWin32Helper.h"

#define AppName "SteinsGate-Client"

USING_NS_CC;
USING_NS_JC;
USING_NS_DETAIL;

AppDelegate::AppDelegate()
	: m_hWndProcHook(nullptr)
{}

AppDelegate::~AppDelegate() 
{
    // 여기서 코코스관련 오브젝트 삭제되도록 하면 릭 발생위험 있음.
    // 예를 들어서 Label의 경우 _fontAtlas 멤버 변수가 포함되어있는데
    // 라벨같은 코코스 오브젝트는 메인 함수의 Application->run() 함수 마무리단계에서 FontAtlasCache::purgeCachedData() 함수의 호출로 해제되도록 되어있다.
    // 근데 내가 코코스 게임엔진의 모든 리소스가 종료되고 나서도 코코스 오브젝트 레퍼런스를 유지하고 있어버리면
    // 일일히 찾아서 해제해줄 수가 없다.
    // 따라서 WorldScene 삭제시 해제해주도록 하자.
    if (m_hWndProcHook != nullptr)
		UnhookWindowsHookEx(m_hWndProcHook);
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}



bool AppDelegate::applicationDidFinishLaunching() {

    FileUtils::getInstance()->setPopupNotify(false);    // 파일못찾은 경우 알람 안하도록 함
    SGConsole::SetSize(1200, 800);

    SGDataManager* pDataManager = SGDataManager::get();
    CoreCommon_v = pDataManager->getCommonInfo(1);
    CoreClient_v = pDataManager->getClientInfo(1);

    CreateOpenGLWindow();
	InitializeJCore();
    InitializeCommonCore();
    InitializeClientCore();
    InitializeDefaultLogger();
    InitializeClientLogo();
    CreateWorldScene();
    
    
    
    return true;
}

void AppDelegate::CreateOpenGLWindow() {

    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    Rect viewRect = { 0, 0, CoreClient_v->ResolutionWidth, CoreClient_v->ResolutionHeight };

    if (glview == nullptr) {
        glview = CoreClient_v->FullScreen ? 
            GLViewImpl::createWithFullScreen(AppName) :
            GLViewImpl::createWithRect(AppName, viewRect, CoreClient_v->GameScale, true);
        
        director->setOpenGLView(glview);
    }

    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 120);
    glview->setDesignResolutionSize(viewRect.size.width, viewRect.size.height, ResolutionPolicy::NO_BORDER);
    //glview->setDesignResolutionSize(CoreInfo_v->ResolutionWidth, CoreInfo_v->ResolutionHeight, ResolutionPolicy::SHOW_ALL);
    director->setContentScaleFactor(1.0f);

    SGWin32Helper::LazyInit();
}

void AppDelegate::CreateWorldScene() {
    auto scene = CoreWorld_v;
    scene->setAnchorPoint(Vec2::ZERO);
    DebugAssertMsg(scene, "월드씬 생성에 실패했습니다.");
    Director::getInstance()->runWithScene(scene);
}

static LRESULT CALLBACK GLFWWindowHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    Scene* runningScene = Director::getInstance()->getRunningScene();

    /*if (runningScene != nullptr) {
        static_cast<SGWorldScene*>(runningScene)->onWndMessageReceived(code, wParam, lParam);
    }*/

    _LogDebug_("%d", code);

    return ::CallNextHookEx(NULL, code, wParam, lParam);
}

void AppDelegate::InitializeWindowProcedure() {
    const HWND hWndCocos = Director::getInstance()->getOpenGLView()->getWin32Window();
    const DWORD dwThreadId = GetWindowThreadProcessId(hWndCocos, NULL);
    const HINSTANCE hInstance = ::GetModuleHandleW(nullptr);
    //m_hWndProcHook = SetWindowsHookEx(WH_CALLWNDPROC, GLFWWindowHookProc, hInstance, dwThreadId);

    //if (m_hWndProcHook == nullptr)
        //_LogError_("코코스 윈도우 프로시저 후킹에 실패했습니다.");
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}

