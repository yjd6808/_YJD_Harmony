/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include <SteinsGate/Research/AppDelegate.h>
#include <SteinsGate/Research/SGDataManager.h>

#include <JCore/Debug/MemoryLeakDetector.h>


int main()
{
    JCore::MemoryLeakDetector detector;
    detector.StartDetect();
    AppDelegate app;
    int iResult = cocos2d::Application::getInstance()->run();
    printf("릭 %d(코코스땜에 제대로 안나옴) \n", detector.StopDetect());
    return iResult;
    
}
