﻿/*
 *	작성자 : 윤정도
 */

#pragma once


#include <JCore/Container/Vector.h>
#include <JNetwork/Worker.h>

NS_JNET_BEGIN

class WorkerManager final
{
private:	// IOCP 클래스에서만 삭제 가능하도록 한다.
	WorkerManager(int threadCount) : m_Workers(threadCount), m_pHandles(nullptr) {}
	~WorkerManager() {
		for (int i = 0; i < m_Workers.Size(); i++) {
			if (CloseHandle(m_pHandles[i]) == FALSE) {
				DebugAssertMsg(false, "워커 매니저 핸들을 닫는데 실패하였습니다.");
			}

			DeleteSafe(m_Workers[i]);
		}
		DeleteArraySafe(m_pHandles);
	}

	template <typename TWorker, typename... Args>
	static WorkerManager* Create(int threadCount, Args&&... args) {
		WorkerManager* pManager = new WorkerManager(threadCount);
		pManager->m_pHandles = new WinHandle[threadCount];

		for (int i = 0; i < threadCount; i++) {
			pManager->m_pHandles[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
			pManager->m_Workers.PushBack(new TWorker(JCore::Forward<Args>(args)...));
		}

		return pManager;
	}

	void Run(void* param = nullptr) {
		m_Workers.Extension().ForEach([param](Worker* worker) { 
			worker->Run(param);
		});
	}

	void Join() const {
		for (int i = 0; i < m_Workers.Size(); i++) {
			m_Workers[i]->JoinWait(m_pHandles[i]);
		}

		// 모든 IOCPWorker 쓰레드들이 종료할때까지 기다린다.
		WaitForMultipleObjects(m_Workers.Size(), m_pHandles, TRUE, INFINITE);

		for (int i = 0; i < m_Workers.Size(); i++) {
			ResetEvent(m_pHandles[i]);
			m_Workers[i]->Join();
		}
	}

private:
	JCore::Vector<Worker*> m_Workers;
	WinHandle* m_pHandles;
	
	friend class IOCP;
};

NS_JNET_END