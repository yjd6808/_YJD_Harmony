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

	void ProcessUpdate(const JCore::TimeSpan& _elapsed);
	void ProcessOrder(CenterOrder_t _order);

	virtual void LaunchServer();
	virtual void StopServer();

	virtual CommonServer* GetMainTcp() const = 0;
	virtual CommonSession* GetSessionFromContainer(int _handle) = 0;
	virtual SGISessionContainer* GetSessionContainer(ServerType_t _type) = 0;

	bool AddUpdatable(int _id, IUpdatable* _pUpdatable);

protected:
	virtual void InitializeIOCP() = 0;
	virtual void InitializeBufferPool() = 0;
	virtual void InitializeServer() = 0;
	virtual void InitializeParser();

	virtual void OnUpdate(const JCore::TimeSpan& _elapsed) = 0;

	SGNormalLock serverBootLock_;
	SGCommandParser* pParser_;
	UpdatableCollection updatableCollection_;

	inline static JCore::AtomicInt UpdatableSeq;
};
