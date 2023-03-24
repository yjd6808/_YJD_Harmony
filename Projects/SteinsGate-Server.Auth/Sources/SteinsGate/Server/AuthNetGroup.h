/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <Jnetwork/NetGroup.h>

#include <SteinsGate/Server/AuthServer.h>
#include <SteinsGate/Server/AuthEventListener.h>


class AuthEventListener;
class AuthNetGroup final : public SGNetGroup
{
public:
	AuthNetGroup();
	~AuthNetGroup() override;
	void Initialize() override;
private:
	SGTcpServerWPtr m_spAuthServer;
	AuthEventListener m_AuthEventListener;
};


