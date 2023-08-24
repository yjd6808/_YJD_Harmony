/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:13:58 PM
 * =====================
 *
 */

#include "Research.h"
#include <JNetwork/Research/UdpClientNetGroup.h>
#include <JNetwork/Research/Command.h>

USING_NS_JC;

NS_JNET_BEGIN

UdpClientNetGroup::UdpClientNetGroup(const JCore::String& name)
	: NetGroup(StringUtil::Format("%s 그룹", name))
{}

UdpClientNetGroup::~UdpClientNetGroup() {
	UdpClientNetGroup::Finalize();
}

void UdpClientNetGroup::Initialize() {
	CreateIocp(8);
	CreateBufferPool({
		{ 8192, 1 }
	});

	RunIocp();

	UdpClientPtr spUdpClient = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool);

	AddHost(1, spUdpClient);

	m_spUdpClient = spUdpClient;
	m_spUdpClient->SetEventListener(dbg_new ClientListener{ m_szName });
}

NS_JNET_END