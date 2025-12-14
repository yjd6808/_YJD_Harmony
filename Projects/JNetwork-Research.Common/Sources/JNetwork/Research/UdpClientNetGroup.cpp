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

//////////////////////////////////////////////////////////////////////////////////////////
UdpClientNetGroup::UdpClientNetGroup(const JCore::String& _name)
	: NetGroup(StringUtil::Format("%s 그룹", _name))
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UdpClientNetGroup::~UdpClientNetGroup()
{
	UdpClientNetGroup::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClientNetGroup::Initialize()
{
	CreateIocp(8);
	CreateBufferPool({
		{ 8192, 1 }
	});

	RunIocp();

	UdpClientPtr pUdpClient = MakeShared<UdpClient>(pIocp_, pBufferPool_);

	AddHost(1, pUdpClient);

	udpClient_ = pUdpClient;
	udpClient_->SetEventListener(dbg_new ClientListener{ name_ });
}

NS_JNET_END
