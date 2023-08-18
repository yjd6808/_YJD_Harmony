/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include "Tutturu.h"
#include "SGApplication.h"

int main() {
    int iResult = -1;
    {
        SGApplication app;
        iResult = cocos2d::Application::getInstance()->run();
    }
    return iResult;
    
}
