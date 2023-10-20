#include "Pch.h"
#include "AppDelegate.h"

#include <JNetwork/Winsock.h>

#include <TF/Client/Game/Scene/LoginScene.h>

using namespace JNetwork;

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}


// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
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

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}
