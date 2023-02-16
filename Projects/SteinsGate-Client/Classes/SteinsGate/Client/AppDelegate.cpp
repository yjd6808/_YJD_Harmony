#include "Tutturu.h"
#include "AppDelegate.h"

#include <SteinsGate/Client/SGWorldScene.h>
#include <SteinsGate/Client/SGActorBox.h>
#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGActorListenerManager.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGUIManager.h>

#include "SGUIManager.h"

#define AppName "SteinsGate-Client"

USING_NS_CC;

AppDelegate::AppDelegate()
{
    
    
}

AppDelegate::~AppDelegate() 
{
    delete SGUIManager::get();
    delete SGActorBox::get();
    delete SGActorListenerManager::get();
    delete SGHostPlayer::get();
    delete SGDataManager::get();
    delete SGImagePackManager::get();
    delete SGGlobal::get();
}

void AppDelegate::initGLContextAttrs()
{
    
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    
    SGDataManager* pDataManager = SGDataManager::get();
    SGClientInfo* pClientInfo = pDataManager->getClientInfo();

    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    
    if (glview == nullptr) {
        glview = GLViewImpl::createWithRect(AppName, SGRect(0, 0, pClientInfo->ResolutionWidth, pClientInfo->ResolutionHeight));
        director->setOpenGLView(glview);
    }
    
    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 120);
    glview->setDesignResolutionSize(pClientInfo->ResolutionWidth, pClientInfo->ResolutionHeight, ResolutionPolicy::NO_BORDER);
    director->setContentScaleFactor(1.0f);

    SGGlobal::get();
    SGImagePackManager::get();
    SGDataManager::get();
    SGHostPlayer::get();
    SGActorListenerManager::get();
    SGActorBox::get();
    SGUIManager::get();
    auto scene = SGWorldScene::get();
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
