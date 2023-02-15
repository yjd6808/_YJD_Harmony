/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include "Tutturu.h"

#include <SteinsGate/Client/AppDelegate.h>
#include <JCore/Debug/MemoryLeakDetector.h>

USING_NS_JC;

int main()
{
//    _CrtSetBreakAlloc(138);
    // Detail::__JCoreInitialize()
    //{
    //    SGGlobal::get();
    //    SGImagePackManager::get();
    //    SGDataManager::get();
    //    SGHostPlayer::get();
    //    SGActorListenerManager::get();
    //    SGActorBox::get();

    //    delete SGActorBox::get();
    //    delete SGActorListenerManager::get();
    //    delete SGHostPlayer::get();
    //    delete SGDataManager::get();
    //    delete SGImagePackManager::get();
    //    delete SGGlobal::get();
    //}

    int iResult = -1;
    {
        AppDelegate app;
        iResult = cocos2d::Application::getInstance()->run();
    }
    return iResult;
    
}
