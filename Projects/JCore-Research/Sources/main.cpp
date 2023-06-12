#include "header.h"


class SchedulerTask;
using SchedulerTaskCallback = Action<SchedulerTask*>;
class SchedulerTask 
{
public:
	virtual ~SchedulerTask() = default;
	virtual void OnScheduled(SchedulerTask* task) {}
	virtual DateTime At() const = 0;
	virtual TimeSpan Interval() const = 0;
	virtual DateTime NextCallTime() const = 0;
	virtual bool CanNextCall() const = 0;
	virtual bool HasCallback() const { return false; }
	virtual void CallCallback() { return OnScheduled(this); }
};

template <typename TCallback>
constexpr bool IsSchedulerTaskCallback_v = IsCallable_v<TCallback> && 
			IsSameType_v<CallableSignatureReturn_t<TCallback>, void> &&
			IsSameType_v<CallableSignatureParam_t<TCallback>, Detail::ParameterPack<SchedulerTask*>>;

class SchedulerTaskOnce : public SchedulerTask
{
public:
	template <typename TCallback, DefaultEnableIf_t<IsSchedulerTaskCallback_v<TCallback>>>
	SchedulerTaskOnce(DateTime at, TCallback&& callback)
		: m_At(at)
		, m_Callback(Forward<TCallback>(callback))
	{}


	DateTime At() const override { return m_At; }
	TimeSpan Interval() const override { return 0; }
	DateTime NextCallTime() const override { return m_At; }
	bool CanNextCall() const override { return false; }
	void CallCallback() override { m_Callback(this); }
private:
	DateTime m_At;
	SchedulerTaskCallback m_Callback;
};

class SchedulerTaskRepeat : public SchedulerTask
{
public:
	template <typename TCallback>
	SchedulerTaskRepeat(DateTime at, TimeSpan interval, TCallback&& callback, Int32U repeat = INFINITE)
		: m_At(at)
		, m_Interval(interval)
		, m_Callback(Forward<TCallback>(callback))
		, m_uiCurRepeat(0)
		, m_uiMaxRepeat(repeat)
	{}

	DateTime At() const override { return m_At; }
	TimeSpan Interval() const override { return m_Interval; }
	DateTime NextCallTime() const override { return m_At + m_Interval; }
	bool CanNextCall() const override { return m_uiCurRepeat < m_uiMaxRepeat; }
	void CallCallback() override {
		m_At.Tick = DateTime::Now().Tick;
		m_uiCurRepeat += 1;
		m_Callback(this);
	}
private:
	DateTime m_At;
	TimeSpan m_Interval;
	SchedulerTaskCallback m_Callback;
	Int32U m_uiCurRepeat;
	Int32U m_uiMaxRepeat;
};




/*
 * 필요한 기능
 * - 특정 시각기준으로 일정시간 간격으로 호출
 * - 현재 시각기준으로 일정시간 간격으로 호출
 * - 특정 시각기준으로 한번만 호출
 */

class Scheduler 
{
	using TaskList = Vector<SchedulerTask*>;

public:
	enum class JoinStrategy
	{
		WaitAllTasks,			// 대기중인 작업이 완료될떄가지 기다림
		WaitOnlyRunningTask		// 실행중인 작업만 기다림
	};

	enum class State
	{
		Running,
		JoinWaitAllTasks,
		JoinWaitOnlyRunningTask,
		Joined
	};

	Scheduler(int threadCount)
		: m_pThreadPool(dbg_new ThreadPool(threadCount))
		, m_SchedulingThread(std::bind(&Scheduler::SchedulingRoutine, this))
		, m_eState(State::Running)
	{}
	~Scheduler() {
		JCORE_DELETE_SAFE(m_pThreadPool);
	}

	template <typename TCallback>
	static constexpr void ValidateCallbackType() { static_assert(IsSchedulerTaskCallback_v<RemoveQulifierReference_t<TCallback>>, "... TCallback is not SchedulerTaskCallBack type (std::function<void(SchedulerTask*)>)"); }

	template <typename TCallback>
	void IntervalAt(TCallback&& callback, DateTime at, TimeSpan interval, Int32U repeat = INFINITE) {
		ValidateCallbackType<TCallback>();

		AddTask(dbg_new SchedulerTaskRepeat {
			at,
			interval,
			Forward<TCallback>(callback),
			repeat
		});
	}

	template <typename TCallback>
	void Interval(TCallback&& callback, TimeSpan interval, Int32U repeat = INFINITE) {
		ValidateCallbackType<TCallback>();

		AddTask(dbg_new SchedulerTaskRepeat {
			DateTime::Now(),
			interval,
			Forward<TCallback>(callback),
			repeat
		});
	}

	template <typename TCallback>
	void OnceAt(TCallback&& callback, DateTime at) {
		ValidateCallbackType<TCallback>();

		AddTask(dbg_new SchedulerTaskOnce {
			at,
			Forward<TCallback>(callback)
		});
	}

	template <typename TCallback>
	void Once(TCallback&& callback) {
		ValidateCallbackType<TCallback>();

		AddTask(dbg_new SchedulerTaskOnce {
			DateTime::Now(),
			Forward<TCallback>(callback)
		});
	}

	void AddTask(SchedulerTask* task) {
		const Int64U uiTick = task->At().Tick;
		{
			NormalLockGuard guard(m_Lock);
			if (m_eState != State::Running) {
				DebugAssertMsg(m_eState == State::Running, "스케쥴러가 Running상태가 아닌데 Task삽입을 시도했습니다.");
				delete task;
				return;
			}

			if (!m_tmTasks.Exist(uiTick)) {
				m_tmTasks.Insert(uiTick, dbg_new TaskList{ task });
			} else {
				m_tmTasks[uiTick]->PushBack(task);
			}
		}
		m_CondVar.NotifyOne();
	}

	int WaitingTaskCount() {
		NormalLockGuard guard(m_Lock);
		return WaitingTaskListCountRaw();
	}

	bool HasRunningTask() {
		return m_pRunningTask != nullptr;
	}


	void Join(JoinStrategy strategy = JoinStrategy::WaitOnlyRunningTask) {
		{
			NormalLockGuard guard(m_Lock);
			m_eState = strategy == JoinStrategy::WaitOnlyRunningTask ? State::JoinWaitOnlyRunningTask : State::JoinWaitAllTasks;
		}

		m_CondVar.NotifyOne();
		m_SchedulingThread.Join();
		m_pThreadPool->Join();

		{
			// NextCall이 있는 Task들의 경우 때문에 Notify이전에 ClearWaitingTaskListRaw()를 호출해서
			// 대기중인 작업들을 정리해서는 안된다.
			NormalLockGuard guard(m_Lock);
			ClearWaitingTaskListRaw();
		}
	}
protected:
	void SchedulingRoutine() {
		TaskList* pvPendingTasks;			// 아직 시간이 되지 않아 실행대기중인 작업
		TaskList vScheduledTasks;			// 시간이 만료되어 실행되어야할 작업

		bool bExit = false;
		bool bHasRemainTasks = false;

		auto fnWait = [this, &bExit, &bHasRemainTasks]()->bool{
			bExit = false;
			bHasRemainTasks = m_tmTasks.Size() > 0;

			if (m_eState == State::JoinWaitAllTasks) {
				bExit = bHasRemainTasks ? false : true;
			} else if (m_eState == State::JoinWaitOnlyRunningTask) {
				bExit = true;
			}

			return bExit || bHasRemainTasks;
		};

		NormalLockGuard guard(m_Lock);

		for (;;) {
			const bool bHasInitTasks = m_tmTasks.TryGetFirstValue(pvPendingTasks);
			if (bHasInitTasks) {
				m_CondVar.WaitUntil(guard, pvPendingTasks->At(0)->At(), Move(fnWait));
			} else {
				m_CondVar.Wait(guard, Move(fnWait));
			}

			if (bExit) {
				break;
			}

			ExecuteTasks(guard, vScheduledTasks);
		}

		m_eState = State::Joined;
	}

	void ExecuteTasks(NormalLockGuard& guard, TaskList& scheduledTasks) {
		const int iTaskCount = PopTasks(scheduledTasks);

		for (int i = 0; i < iTaskCount; ++i) {
			SchedulerTask* task = scheduledTasks[i];
			m_pRunningTask = task;
			guard.Unlock();
			task->CallCallback();
			guard.Lock();

			if (!task->CanNextCall()) {
				delete task;
				continue;
			}

			Int64U uiNextCallTime = task->NextCallTime().Tick;
			if (!m_tmTasks.Exist(uiNextCallTime)) {
				m_tmTasks.Insert(uiNextCallTime, dbg_new TaskList{ task });
			} else {
				m_tmTasks[uiNextCallTime]->PushBack(task);
			}
		}

		m_pRunningTask = nullptr;
	}

	
	int PopTasks(JCORE_OUT Vector<SchedulerTask*>& tasks) {
		tasks.Clear();

		if (m_tmTasks.Size() == 0) {
			return 0;
		}

		// 현재 시각을 기준으로 시간이 만료된 작업들을 가져온다.
		auto cur = m_tmTasks.Begin();
		Int64U* pNotExpiredKey = m_tmTasks.UpperBoundKey(DateTime::Now().Tick); // 아직 시간이 만료되지 않은 첫 원소

		Int64U uiNotExpiredKey = MaxInt64U_v;
		if (pNotExpiredKey) uiNotExpiredKey = *pNotExpiredKey;


		while (cur->HasNext()) {
			auto& curValue = cur->Current();

			const Int64U& expiredTaskKey = curValue.Key;
			const TaskList* expiredTaskList = curValue.Value;

			if (expiredTaskKey >= uiNotExpiredKey) {
				break;
			}

			const bool bRemoved = m_tmTasks.RemoveByIterator(cur);
			DebugAssert(bRemoved);

			for (int i = 0; i < expiredTaskList->Size(); ++i) {
				tasks.PushBack(expiredTaskList->At(i));
			}

			delete expiredTaskList;
		}

		const int iPoppedSize = tasks.Size();
		return iPoppedSize;
	}

	int WaitingTaskListCountRaw() {
		int iCount = 0;
		m_tmTasks.ForEachValue([&iCount](TaskList* taskList) { iCount += taskList->Size(); });
		return iCount;
	}

	void ClearWaitingTaskListRaw() {
		auto it = m_tmTasks.Begin();
		while (it->HasNext()) {

			TaskList* pList = it->Current().Value;
			pList->ForEach([](auto task) { delete task; });
			delete pList;
			m_tmTasks.RemoveByIterator(it);
		}
	}


	TreeMap<Int64U, TaskList*> m_tmTasks;
	ConditionVariable m_CondVar;
	NormalLock m_Lock;
	ThreadPool* m_pThreadPool;
	Thread m_SchedulingThread;
	Atomic<SchedulerTask*> m_pRunningTask;
	State m_eState;
};

int R(int min, int max) { return Random::GenerateInt(min, max); }



USING_NS_STD_CHRONO;

int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();

	int i = 0;
	for (;;)
	{
		Console::WriteLine(" ======================= [%d] ======================= ", ++i);
		Scheduler s(6);

		s.IntervalAt([](SchedulerTask* task) {
			Thread::Sleep(R(10, 100));
			Console::WriteLine("A");
		}, DateTime::Now() + TimeSpan::FromMiliSeocnd(R(0, 300)), TimeSpan::FromMiliSeocnd(R(50, 400)), 5);

		s.IntervalAt([](SchedulerTask* task) {
			Thread::Sleep(R(1, 100));
			Console::WriteLine("B");
		}, DateTime::Now() + TimeSpan::FromMiliSeocnd(R(0, 150)), TimeSpan::FromMiliSeocnd(R(10, 50)), 5);

		s.IntervalAt([](SchedulerTask* task) {
			Thread::Sleep(R(1, 50));
			Console::WriteLine("C");
		}, DateTime::Now(), TimeSpan::FromMiliSeocnd(R(0, 300)), 5);

		//Thread::Sleep(1);
		s.Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);

		if (i == 100)
			break;
	}
	return 0;
}

