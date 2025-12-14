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
	WorkerGroup(int _threadCount)
	: workers_(_threadCount)
	, handles_(_threadCount)
	{
	}

	~WorkerGroup() noexcept
	{
		for (int i = 0; i < workers_.Size(); i++)
		{
			delete workers_[i];
		}
	}

	template <typename TWorker, typename... Args>
	static WorkerGroup* Create(int _threadCount, Args&&... _args)
	{
		USING_NS_JC;

		WorkerGroup* pManager = dbg_new WorkerGroup(_threadCount);

		for (int i = 0; i < _threadCount; i++)
		{
			// TODO: 이벤트에 동일한 이름부여하면 IOCP를 여러개 생성 후 join할 때 이상하게 동작되는 경우가 있다. 원인파악 할 것
			// 발생했던 이상한 동작
			// 1번 IOCP가 워커1, 워커2라는 이름으로 이벤트를 생성한 후
			// 2번 IOCP가 워커1, 워커2라는 이름으로 이벤트를 생성하고 join을하면 WaitForMultipleObjects가 곧바로 기다리지도 않고 정상 리턴을 해버림.
			// 다시 재빌하드니까 제대로됨. 뭐지? 이룸문제가 아닌건가

			String workerName = StringUtil::Format("%s 워커(%d)", typeid(TWorker).name(), i);

			pManager->workers_.PushBack(dbg_new TWorker(Forward<Args>(_args)...));
			pManager->handles_.EmplaceBack(false, workerName.Source());
		}

		return pManager;
	}

	void Run(void* _pParam = nullptr)
	{
		workers_.Extension().ForEach([_pParam](Worker* _pWorker)
		{
			_pWorker->Run(_pParam);
		});
	}

	void Join() const
	{
		for (int i = 0; i < workers_.Size(); i++)
		{
			workers_[i]->JoinWait(&handles_[i]);
		}

		Int32UL waitResult;
		if (JCore::WaitHandle::WaitAll(handles_, &waitResult) == false)
		{
			_NetLogError_("워커그룹 Join시도중 오류 발생 (%ul)", waitResult);
		}

		for (int i = 0; i < workers_.Size(); i++)
		{
			handles_[i].Reset();
			Worker* pWorker = workers_[i];
			pWorker->Join();
		}
	}

	void SetName(const JCore::String& _name)
	{
		name_ = _name;
	}

	const JCore::String& GetName() const
	{
		return name_;
	}

private:
	JCore::Vector<Worker*> workers_;
	JCore::Vector<JCore::AutoResetEvent> handles_;
	JCore::String name_;

	friend class IOCP;
};

NS_JNET_END
