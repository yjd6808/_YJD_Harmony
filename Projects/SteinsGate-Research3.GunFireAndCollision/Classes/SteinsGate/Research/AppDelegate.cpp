#include "AppDelegate.h"

#include <SteinsGate/Research/World.h>
#include <SteinsGate/Research/TestScene.h>
#include <SteinsGate/Research/PackLoader.h>

#include "SteinsGate/Common/Engine/RectEx.h"

#define AppName "SteinsGate-Research.AvatarAnimation"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1280, 720);

AppDelegate::AppDelegate()
{
    PackLoader::GetInstance();
}

AppDelegate::~AppDelegate() 
{
    delete PackLoader::GetInstance();
    delete World::getInstance();
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {



    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::createWithRect(AppName, cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
        director->setOpenGLView(glview);
    }

    

    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 60);
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
    director->setContentScaleFactor(1.0f);

    auto scene = TestScene::createScene();
    director->runWithScene(scene);
    World::getInstance()->onSceneLoaded(scene);
    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}
