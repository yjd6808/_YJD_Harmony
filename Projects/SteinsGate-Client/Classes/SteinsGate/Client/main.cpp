/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include "Tutturu.h"

#include <SteinsGate/Client/AppDelegate.h>
#include <SteinsGate/Client/GameCore.h>

USING_NS_JC;
USING_NS_DETAIL;

int main()
{
    InitializeJCore();
    InitializeCommonCore();
    InitializeDefaultLogger();
    InitializeClientLogo();
    InitializeClientCore();

    int iResult = -1;
    {
        AppDelegate app;
        iResult = cocos2d::Application::getInstance()->run();
    }

    FinalizeClientCore();
    FinalizeCommonCore();
    FinalizeDefaultLogger();
    return iResult;
    
}
