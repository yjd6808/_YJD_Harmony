#include "Tutturu.h"
#include "AppDelegate.h"
#include "GameCoreHeader.h"

#define AppName "SteinsGate-Client"

USING_NS_CC;
USING_NS_JC;
USING_NS_DETAIL;

AppDelegate::AppDelegate()
{
    
    
}

AppDelegate::~AppDelegate() 
{
    // 여기서 코코스관련 오브젝트 삭제되도록 하면 릭 발생위험 있음.
    // 예를 들어서 Label의 경우 _fontAtlas 멤버 변수가 포함되어있는데
    // 이녀석은 메인 함수의 Application->run() 함수 마무리단계에서 FontAtlasCache::purgeCachedData() 함수의 호출로 해제되도록 되어있다.
    // 근데 내가 코코스 게임엔진의 모든 리소스가 종료되고 나서도 코코스 오브젝트 레퍼런스를 유지하고 있어버리면
    // 일일히 찾아서 해제해줄 수가 없다.
    // 따라서 WorldScene 삭제시 해제해주도록 하자.
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    
    SGDataManager* pDataManager = SGDataManager::get();

    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    CoreInfo_v = SGDataManager::get()->getClientInfo(1);
    
    if (glview == nullptr) {
        glview = GLViewImpl::createWithRect(AppName, SGRect(0, 0, CoreInfo_v->ResolutionWidth, CoreInfo_v->ResolutionHeight));
        director->setOpenGLView(glview);
    }
    
    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 120);
    glview->setDesignResolutionSize(CoreInfo_v->ResolutionWidth, CoreInfo_v->ResolutionHeight, ResolutionPolicy::NO_BORDER);
    director->setContentScaleFactor(1.0f);

    FileUtils::getInstance()->setPopupNotify(false);    // 파일못찾은 경우 알람 안하도록 함
    SGConsole::SetSize(1200, 800);

    InitializeJCore();
    InitializeCommonCore();
    InitializeDefaultLogger();
    InitializeClientCore();

    InitializeServerAuthLogo();
    _LogPlain_(Logo_v);
    InitializeServerLobbyLogo();
    _LogPlain_(Logo_v);
    InitializeServerGameLogo();
    _LogPlain_(Logo_v);
    InitializeServerCenterLogo();
    _LogPlain_(Logo_v);

    auto scene = CoreWorld_v;
    scene->setAnchorPoint(Vec2::ZERO);
    DebugAssertMsg(scene, "월드씬 생성에 실패했습니다.");
    director->runWithScene(scene);
    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}
