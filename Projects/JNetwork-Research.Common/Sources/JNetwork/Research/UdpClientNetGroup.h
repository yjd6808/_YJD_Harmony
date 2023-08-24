/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:04:21 PM
 * =====================
 * UDP 클라이언트 테스트
 */


#pragma once

#include <JNetwork/Host/UdpClient.h>
#include <JNetwork/NetGroup.h>

#include <JNetwork/Research/ClientListener.h>

NS_JNET_BEGIN

class UdpClientNetGroup : public NetGroup
{
public:
	UdpClientNetGroup(const JCore::String& name);
	~UdpClientNetGroup() override;

	void Initialize() override;
	UdpClient* Get() { return m_spUdpClient.GetPtr(); }
private:
	UdpClientWPtr m_spUdpClient;
};

NS_JNET_END