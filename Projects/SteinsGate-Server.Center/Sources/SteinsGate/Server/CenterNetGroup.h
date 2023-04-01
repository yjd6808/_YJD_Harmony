/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <Jnetwork/NetGroup.h>

#include <SteinsGate/Server/CenterServer.h>
#include <SteinsGate/Server/CenterEventListener.h>


class CenterEventListener;
class CenterNetGroup final : public SGNetGroup
{
public:
	CenterNetGroup();
	~CenterNetGroup() override;
	void Initialize() override;
private:
	SGTcpServerWPtr m_spCenterServer;
	CenterEventListener m_CenterEventListener;
};


