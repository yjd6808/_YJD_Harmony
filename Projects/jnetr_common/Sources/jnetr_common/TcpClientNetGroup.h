/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:04:21 PM
 * =====================
 * TCP 클라이언트 테스트
 */


#pragma once

#include <jnet/Host/TcpClient.h>
#include <jnet/NetGroup.h>

#include <jnetr_common/ClientListener.h>

NS_JNET_BEGIN

class TcpClientNetGroup : public NetGroup
{
public:
	TcpClientNetGroup();
	~TcpClientNetGroup() override;

	void Initialize() override;
	TcpClient* GetLogin()
	{
		return loginClient_.GetPtr();
	}
	TcpClient* GetChannel()
	{
		return channelClient_.GetPtr();
	}
	TcpClient* GetGame()
	{
		return gameClient_.GetPtr();
	}
private:
	TcpClientWPtr loginClient_;
	TcpClientWPtr channelClient_;
	TcpClientWPtr gameClient_;
};

NS_JNET_END
