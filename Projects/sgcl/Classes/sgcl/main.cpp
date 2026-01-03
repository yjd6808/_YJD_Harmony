/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include "Core.h"
#include "SteinsGateApp.h"

//////////////////////////////////////////////////////////////////////////////////////////
int main(int _argc, char** _argv)
{
	struct Check : jc::ObjectPool<Check> {
		int a = 20;
	};

	Check* a = dbg_new Check();
	delete a;
	int result = -1;
	{
		SteinsGateApp app(_argc, _argv);
		result = app.run();
	}
	return result;
}
