/*
 *	작성자 : 윤정도
 */

#pragma once


#include <JCore/Container/Vector.h>
#include <JCore/Sync/AutoResetEvent.h>
#include <JNetwork/Worker.h>

NS_JNET_BEGIN

class WorkerGroup final
{
public:
	WorkerGroup(int threadCount)
		: m_vWorkers(threadCount, nullptr)
		, m_vHandles(threadCount, nullptr)
	{}


	template <typename TWorker, typename... Args>
	static WorkerGroup* Create(int threadCount, Args&&... args) {
		USING_NS_JC;

		WorkerGroup* pManager = dbg_new WorkerGroup(threadCount);

		for (int i = 0; i < threadCount; i++) {
			pManager->m_vWorkers[i] = MakeShared<TWorker>(Forward<Args>(args)...);
			pManager->m_vHandles[i] = MakeShared<AutoResetEvent>(false, StringUtil::Format("%s 워커(%d)", typeid(TWorker).name(), i).Source());
		}

		return pManager;
	}

	void Run(void* param = nullptr) {
		m_vWorkers.Extension().ForEach([param](WorkerPtr& worker) { 
			worker->Run(param);
		});
	}

	void Join() const {
		for (int i = 0; i < m_vWorkers.Size(); i++) {
			m_vWorkers[i]->JoinWait(m_vHandles[i].GetPtr());
		}

		JCore::WaitHandle::WaitAll(m_vHandles[0].GetPtr(), m_vHandles.Size());

		for (int i = 0; i < m_vWorkers.Size(); i++) {
			m_vHandles[i]->Reset();
			m_vWorkers[i]->Join();
		}
	}

private:
	JCore::Vector<WorkerPtr> m_vWorkers;
	JCore::Vector<JCore::WaitHandlePtr> m_vHandles;
	
	friend class IOCP;
};

NS_JNET_END