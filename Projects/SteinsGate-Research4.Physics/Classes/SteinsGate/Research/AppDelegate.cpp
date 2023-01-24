#include "AppDelegate.h"

#include <SteinsGate/Research/GameScene.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGGlobal.h>

#include "SteinsGate/Common/Engine/RectEx.h"

#define AppName "SteinsGate-Research4.Physics"

USING_NS_CC;

AppDelegate::AppDelegate()
{
    SGGlobal::getInstance();
    SGImagePackManager::getInstance();
    SGDataManager::getInstance();
}

AppDelegate::~AppDelegate() 
{
    delete SGDataManager::getInstance();
    delete SGImagePackManager::getInstance();
    delete SGGlobal::getInstance();
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    SGDataManager* pDataManager = SGDataManager::getInstance();
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

    auto scene = GameScene::createScene();
    scene->setAnchorPoint(Vec2::ZERO);
    director->runWithScene(scene);
    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}
