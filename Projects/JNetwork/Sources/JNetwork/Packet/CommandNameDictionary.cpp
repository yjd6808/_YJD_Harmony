/*
 * 작성자: 윤정도
 * 생성일: 7/3/2023 5:46:32 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Packet/CommandNameDictionary.h>

USING_NS_JC;

NS_JNET_BEGIN

const char* CommandNameDictionary::Get(Cmd_t cmd) const {
	if (!m_CommandNameMap.Exist(cmd)) {
		return "Unknown";
	}

	return m_CommandNameMap.Get(cmd);
}

NS_JNET_END
