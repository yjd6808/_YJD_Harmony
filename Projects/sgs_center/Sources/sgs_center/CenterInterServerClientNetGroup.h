/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 11:42:19 PM
 * =====================
 *
 */


#pragma once


#include <sgs/_Net/NetGroup_InterServ.h>

class CenterInterServerClientNetGroup : public NetGroup_InterServ
{
public:
	CenterInterServerClientNetGroup();

	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeBufferPool() override;
	void InitializeInterServerTcp() override;
	void InitializeInterServerUdp() override;
protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
	bool IsPeerServer() override
	{
		return false;
	}
};
