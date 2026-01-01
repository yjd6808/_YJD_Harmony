/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include "Core.h"
#include "SteinsGateApp.h"

//////////////////////////////////////////////////////////////////////////////////////////
int main(int _argc, char** _argv)
{
	int result = -1;
	{
		jc::Env::InitArgs(_argc, _argv);
		SteinsGateApp app;
		result = cocos2d::Application::getInstance()->run();
	}
	return result;
}
