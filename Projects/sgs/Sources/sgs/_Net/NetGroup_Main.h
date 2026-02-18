/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 1:42:49 PM
 * =====================
 *
 */

#pragma once

#include <jnet/NetGroup.h>
#include <jnet/Host/SessionContainer.h>

#include <sgs/IUpdatable.h>
#include <sgs/UpdatableCollection.h>

NS_SG_BEGIN

class NetServer;
class NetSession;
class NetGroup_Main : public jnet::NetGroup, public IUpdatable, public jc::SingletonSPointer<NetGroup_Main>
{
public:
	NetGroup_Main();
	~NetGroup_Main() override;

	void Initialize() override;
	void Finalize() override;

	virtual void OnUpdate(const jc::TimeSpan& _elapsed);

	void ProcessOrder(CenterOrder_t _order, jc::String _arg);
	void LaunchServer();
	void StopServer();

	bool AddUpdatable(int _id, IUpdatable* _pUpdatable);
	jnet::CommandParser& Parser() { return *pParser_; }

protected:
	static constexpr int MAX_SERVER_COUNT = 10; // ServerType::Max 만큼 서버를 담을 수 있는 배열, 각 인덱스는 ServerType과 매칭됨.
	NetServer* pServerArr_[MAX_SERVER_COUNT]{}; 
	jnet::SessionContainer* pSessionContainerArr_[MAX_SERVER_COUNT]{};
	jc::NormalLock serverBootLock_;
	jnet::CommandParser* pParser_;
	UpdatableCollection updatableCollection_;

	inline static jc::AtomicInt UpdatableSeq;
};

NS_END

#define g_cNetGroup_Main JC_DECL_SINGLETON_BODY(sg::NetGroup_Main)