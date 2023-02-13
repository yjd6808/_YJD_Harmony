/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include "Tutturu.h"

#include <SteinsGate/Client/AppDelegate.h>
#include <SteinsGate/Client/SGDataManager.h>

#include <JCore/Debug/MemoryLeakDetector.h>

#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGGameScene.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGActorListenerManager.h>
#include <SteinsGate/Client/SGGlobal.h>

int main()
{
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    JCore::MemoryLeakDetector detector;
    detector.StartDetect();

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
    printf("릭 %d \n", detector.StopDetect());
    return iResult;
    
}
