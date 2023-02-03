/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include "Tutturu.h"

#include <SteinsGate/Research/AppDelegate.h>
#include <SteinsGate/Research/SGDataManager.h>

#include <JCore/Debug/MemoryLeakDetector.h>

#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/GameScene.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGActorListenerManager.h>
#include <SteinsGate/Research/SGGlobal.h>

int main()
{
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    JCore::MemoryLeakDetector detector;
    detector.StartDetect();

    //{
    //    SGGlobal::getInstance();
    //    SGImagePackManager::getInstance();
    //    SGDataManager::getInstance();
    //    SGPlayer::getInstance();
    //    SGActorListenerManager::getInstance();
    //    SGActorBox::getInstance();

    //    delete SGActorBox::getInstance();
    //    delete SGActorListenerManager::getInstance();
    //    delete SGPlayer::getInstance();
    //    delete SGDataManager::getInstance();
    //    delete SGImagePackManager::getInstance();
    //    delete SGGlobal::getInstance();
    //}

    int iResult = -1;
    {
        AppDelegate app;
        iResult = cocos2d::Application::getInstance()->run();
    }
    printf("릭 %d \n", detector.StopDetect());
    return iResult;
    
}
