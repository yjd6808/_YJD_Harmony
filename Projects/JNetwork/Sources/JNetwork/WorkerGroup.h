﻿/*
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
		: m_vWorkers(threadCount)
		, m_vHandles(threadCount)
	{}
	~WorkerGroup() noexcept {
		for (int i = 0; i < m_vWorkers.Size(); i++) {
			delete m_vWorkers[i];
		}
	}

	template <typename TWorker, typename... Args>
	static WorkerGroup* Create(int threadCount, Args&&... args) {
		USING_NS_JC;

		WorkerGroup* pManager = dbg_new WorkerGroup(threadCount);

		for (int i = 0; i < threadCount; i++) {
			// TODO: 이벤트에 동일한 이름부여하면 IOCP를 여러개 생성 후 join할 때 이상하게 동작되는 경우가 있다. 원인파악 할 것
			// 발생했던 이상한 동작
			// 1번 IOCP가 워커1, 워커2라는 이름으로 이벤트를 생성한 후
			// 2번 IOCP가 워커1, 워커2라는 이름으로 이벤트를 생성하고 join을하면 WaitForMultipleObjects가 곧바로 기다리지도 않고 정상 리턴을 해버림.
			// 다시 재빌하드니까 제대로됨. 뭐지? 이룸문제가 아닌건가

			String workerName = StringUtil::Format("%s 워커(%d)", typeid(TWorker).name(), i);

			pManager->m_vWorkers.PushBack(dbg_new TWorker(Forward<Args>(args)...));
			pManager->m_vHandles.EmplaceBack(false, workerName.Source());
		}

		return pManager;
	}

	void Run(void* param = nullptr) {
		m_vWorkers.Extension().ForEach([param](Worker* worker) { 
			worker->Run(param);
		});
	}

	void Join() const {
		for (int i = 0; i < m_vWorkers.Size(); i++) {
			m_vWorkers[i]->JoinWait(&m_vHandles[i]);
		}

		Int32UL uiResult;
		if (JCore::WaitHandle::WaitAll(m_vHandles, &uiResult) == false) {
			_NetLogError_("워커그룹 Join시도중 오류 발생 (%ul)", uiResult);
		}

		for (int i = 0; i < m_vWorkers.Size(); i++) {
			m_vHandles[i].Reset();
			m_vWorkers[i]->Join();
		}
	}

	void SetName(const JCore::String& name) { m_szName = name; }
	const JCore::String& GetName() const { return m_szName; }

private:
	JCore::Vector<Worker*> m_vWorkers;
	JCore::Vector<JCore::AutoResetEvent> m_vHandles;
	JCore::String m_szName;
	
	friend class IOCP;
};

NS_JNET_END