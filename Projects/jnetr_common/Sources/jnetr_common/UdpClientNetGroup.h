/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:04:21 PM
 * =====================
 * UDP 클라이언트 테스트
 */


#pragma once

#include <jnet/Host/UdpClient.h>
#include <jnet/NetGroup.h>

#include <jnetr_common/ClientListener.h>

NS_JNET_BEGIN

class UdpClientNetGroup : public NetGroup
{
public:
	UdpClientNetGroup(const jc::String& _name);
	~UdpClientNetGroup() override;

	void Initialize() override;
	UdpClient* Get()
	{
		return udpClient_.GetPtr();
	}
private:
	UdpClientWPtr udpClient_;
};

NS_END
