/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include "Core.h"
#include "App.h"

//////////////////////////////////////////////////////////////////////////////////////////
int main(int _argc, char** _argv)
{
	int result = -1;
	{
		App app(_argc, _argv);
		result = app.run();
	}
	return result;
}
