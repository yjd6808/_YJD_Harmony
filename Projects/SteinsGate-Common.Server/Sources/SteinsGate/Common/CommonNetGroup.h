/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 1:42:49 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/NetGroup.h>
#include <JNetwork/Host/SessionContainer.h>

#include <SteinsGate/Common/UpdatableCollection.h>

class CommonServer;
class CommonSession;

class JCORE_NOVTABLE CommonNetGroup : public JNetwork::NetGroup
{
public:
	CommonNetGroup();
	~CommonNetGroup() override;

	void Initialize() override;
	void Finalize() override;

	void ProcessUpdate(const JCore::TimeSpan& elapsed);
	void ProcessOrder(CenterOrder_t order);

	virtual void LaunchServer();
	virtual void StopServer();

	virtual CommonServer* GetMainTcp() const = 0;
	virtual CommonSession* GetSessionFromContainer(int handle) = 0;
	virtual SGISessionContainer* GetSessionContainer(ServerType_t type) = 0;

	bool AddUpdatable(int id, IUpdatable* updatable);
protected:
	virtual void InitializeIOCP() = 0;
	virtual void InitializeBufferPool() = 0;
	virtual void InitializeServer() = 0;
	virtual void InitializeParser();

	virtual void OnUpdate(const JCore::TimeSpan& elapsed) = 0;

	SGNormalLock m_ServerBootLock;
	SGCommandParser* m_pParser;
	UpdatableCollection m_UpdatableCollection;

	inline static JCore::AtomicInt ms_iUpdatableSeq;
};