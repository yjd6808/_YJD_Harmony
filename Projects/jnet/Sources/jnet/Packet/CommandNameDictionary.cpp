/*
 * 작성자: 윤정도
 * 생성일: 7/3/2023 5:46:32 PM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/Packet/CommandNameDictionary.h>

USING_NS_JC;

NS_JNET_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
const char* CommandNameDictionary::Get(Cmd_t _command) const
{
	if (!commandNameMap_.Exist(_command))
	{
		return "Unknown";
	}

	return commandNameMap_.Get(_command);
}

NS_JNET_END
