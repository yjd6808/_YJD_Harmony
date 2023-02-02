/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include <SteinsGate/Research/AppDelegate.h>
#include <SteinsGate/Research/SGDataManager.h>

#include <JCore/Debug/MemoryLeakDetector.h>

#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGImagePackManager.h>


int main()
{
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    JCore::MemoryLeakDetector detector;
    detector.StartDetect();
    /*
    SGGlobal::getInstance();
    SGImagePackManager::getInstance();
    SGDataManager::getInstance();

    delete SGDataManager::getInstance();
    delete SGImagePackManager::getInstance();
    delete SGGlobal::getInstance();
    printf("릭 %d \n", detector.StopDetect());
	*/
    int iResult = -1;
    {
        AppDelegate app;
        iResult = cocos2d::Application::getInstance()->run();
    }
    printf("릭 %d(코코스땜에 제대로 안나옴) \n", detector.StopDetect());
    return iResult;
    
}
