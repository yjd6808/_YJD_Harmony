/*
 * 작성자: 윤정도
 * 생성일: 10/9/2023 7:23:12 PM
 * =====================
 *
 */

#include "Pch.h"
#include "AppDelegate.h"

USING_NS_CC;
USING_NS_JC;
USING_NS_JNET;

int main() {
	DefaultParserType = PacketParser::Command;

    new char; // Force Leak
    int iExit = -1;
    {
        AppDelegate app;
        iExit = Application::getInstance()->run();
    }
    return iExit;
}
