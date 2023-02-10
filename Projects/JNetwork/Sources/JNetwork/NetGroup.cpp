/*
 * 작성자: 윤정도
 * 생성일: 2/10/2023 11:41:19 AM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/NetGroup.h>


NS_JNET_BEGIN

NetGroup::NetGroup()
	: m_vHostList(8)
{
}

void NetGroup::Finalize() {
	m_vHostList.Clear();
	m_spIOCP->Join();
	m_spIOCP->Destroy();
	m_spBufferPool = nullptr;
}


NS_JNET_END