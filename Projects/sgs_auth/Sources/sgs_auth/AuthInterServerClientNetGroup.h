/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 11:42:19 PM
 * =====================
 *
 */


#pragma once

#include <sgs/_Net/NetGroup_InterServ.h>

class AuthInterServerClientNetGroup : public NetGroup_InterServ
{
public:
	AuthInterServerClientNetGroup();
	~AuthInterServerClientNetGroup() override;

	void InitializeParser() override;
	void InitializeIOCP() override;
	void InitializeBufferPool() override;
	void InitializeInterServerTcp() override;
	void InitializeInterServerUdp() override;

protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
};
