/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 1:42:49 PM
 * =====================
 *
 */

#pragma once

#include <jnet/NetGroup.h>
#include <jnet/Host/SessionContainer.h>

#include <sgs/UpdatableCollection.h>

class CommonServer;
class CommonSession;

class JC_NOVTABLE CommonNetGroup : public jnet::NetGroup
{
public:
	CommonNetGroup();
	~CommonNetGroup() override;

	void Initialize() override;
	void Finalize() override;

	void ProcessUpdate(const jc::TimeSpan& _elapsed);
	void ProcessOrder(CenterOrder_t _order);

	virtual void LaunchServer();
	virtual void StopServer();

	virtual CommonServer* GetMainTcp() const = 0;
	virtual CommonSession* GetSessionFromContainer(int _handle) = 0;
	virtual jnet::ISessionContainer* GetSessionContainer(ServerType_t _type) = 0;

	bool AddUpdatable(int _id, IUpdatable* _pUpdatable);

protected:
	virtual void InitializeIOCP() = 0;
	virtual void InitializeBufferPool() = 0;
	virtual void InitializeServer() = 0;
	virtual void InitializeParser();

	virtual void OnUpdate(const jc::TimeSpan& _elapsed) = 0;

	jc::NormalLock serverBootLock_;
	jnet::CommandParser* pParser_;
	UpdatableCollection updatableCollection_;

	inline static jc::AtomicInt UpdatableSeq;
};
