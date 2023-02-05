#include "Tutturu.h"
#include "AppDelegate.h"

#include <SteinsGate/Research/SGWorldScene.h>
#include <SteinsGate/Research/SGActorBox.h>
#include <SteinsGate/Research/SGHostPlayer.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGActorListenerManager.h>
#include <SteinsGate/Research/SGGlobal.h>

#define AppName "SteinsGate-Research6.Fight"

USING_NS_CC;

AppDelegate::AppDelegate()
{
    SGGlobal::get();
    SGImagePackManager::get();
    SGDataManager::get();
    SGHostPlayer::get();
    SGActorListenerManager::get();
    SGActorBox::get();
}

AppDelegate::~AppDelegate() 
{
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
    if(!glview) {
        glview = GLViewImpl::createWithRect(AppName, SGRect(0, 0, pClientInfo->ResolutionWidth, pClientInfo->ResolutionHeight));
        director->setOpenGLView(glview);
    }

    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 120);
    glview->setDesignResolutionSize(pClientInfo->ResolutionWidth, pClientInfo->ResolutionHeight, ResolutionPolicy::NO_BORDER);
    director->setContentScaleFactor(pClientInfo->GameScale);

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
