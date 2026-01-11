#include "Core.h"

#include <sg/_Core/CLIListener.h>

REGISTER_CLI_FUNCTION("exit", CLI_Exit, 
	"프로그램을 종료합니다."
)

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_Exit(int _argc, jc::String* _argv, OUT jc::String& _error)
{
	cocos2d::Director::getInstance()->end();
	return true;
}
