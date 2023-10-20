#include "Pch.h"
#include "AppDelegate.h"

#include <JNetwork/Winsock.h>

#include <TF/Client/Game/Scene/LoginScene.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

AppDelegate::AppDelegate() {}
AppDelegate::~AppDelegate() {}

void AppDelegate::initGLContextAttrs() {
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    Random::EngineInitialize();
    Winsock::Initialize(2, 2);
    InitializeDefaultLogger();
    InitializeNetLogger();
    InitializeCommon();
    CreateOpenGLWindow();
    CreateScene();

    InitializeGameCore();   // 씬 실행후 초기화
    return true;
}

void AppDelegate::applicationDidExit() {
    FinalizeGameCore();
    FinalizeCommon();
    FinalizeNetLogger();
    FinalizeDefaultLogger();
    Winsock::Finalize();
}


void AppDelegate::CreateOpenGLWindow() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();

    if (glview == nullptr) {
        glview = GLViewImpl::createWithRect(Const::Window::ViewName, { 0, 0, Const::Window::Width, Const::Window::Height }, 1.0f, false);
        glview->setDesignResolutionSize(Const::Window::Width, Const::Window::Height, ResolutionPolicy::NO_BORDER);
    }

    director->setOpenGLView(glview);
    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 120);
    director->setContentScaleFactor(1.0f);
}

void AppDelegate::CreateScene() {

    auto scene = LoginScene::create();
    scene->setAnchorPoint(Vec2::ZERO);
    DebugAssertMsg(scene, "월드씬 생성에 실패했습니다.");
    Director::getInstance()->runWithScene(scene);
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}
