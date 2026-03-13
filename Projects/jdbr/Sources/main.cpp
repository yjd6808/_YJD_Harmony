#include <jdb/Core.h>

#include "jc/Threading/Pulser.h"

USING_NS_JC;

int main(int _argc, char** _argv) 
{
	InitializeJCore(_argc, _argv);
	InitializeDefaultLogger();
	InitializeNetLogger();

	jdb::SqlServerDatabaseInfo info;
	info.name_ = "게임DB";
	info.hostName_ = "127.0.0.1";
	info.connPort_ = 1433;
	info.accountId_ = "sa";
	info.accountPass_ = "1234";
	info.dbName_ = "steinsgate";
	info.maxConnection_ = 100;
	info.connPoolSize_ = 10;
	info.iocpThreadCount_ = 4;

	jdb::SqlServerDatabase sqlServerDb;
	if (sqlServerDb.Initialize(info) == false)
		return -1;

	sqlServerDb.SetOnTaskCompletedCallback([](jnet::IOCPTaskAbstract* _pTask)
	{
		if (_pTask->GetType() != IOCP_TASK_TYPE_SQL_SERVER)
			return;

		// PollEvents에서 호출되는 콜백이므로 메인 쓰레드에서만 호출된다.
		jc_assert(Thread::GetMainThreadId() == Thread::GetThreadId());
		auto pTask = (jnet::IOCPTask<jdb::SqlServerQueryPtr>*)_pTask;
		pTask->Wait();
		jdb::SqlServerQuery* pResult = pTask->GetResult().value_.GetPtr();

	});

	// ------------------------------------------
	// insert 테스트
	{
		// auto pInsertQuery = sqlServerDb.Query("INSERT INTO t_account (c_account_id, c_account_pass) VALUES('user03', 'pass03'); ");
		// _u32 rowCount = pInsertQuery->GetRowCount();
		// jc_assert_msg(rowCount == 1, "영향받은 행 갯수가 1이 아닙니다. rowCount: %u", rowCount);
	}


	// ------------------------------------------
	// select 테스트
	constexpr int TCNT = 2;
	bool runningThread[TCNT]{};
	Thread thread[TCNT];
	{
		for (int i = 0; i < TCNT; ++i)
		{
			thread[i].Start([i, &sqlServerDb, &runningThread](void* _param)
			{
				runningThread[i] = true;
				while (runningThread[i])
				{
					auto pQuery = sqlServerDb.QueryAsync(100, "select * from t_account");
				}
			});
		}

		auto pQuery = sqlServerDb.QueryAsync(100, "select * from t_account");
	}

	Console::Write("%s\n", "x키 입력시 종료");

	int updatePerSecond = 10;
	PulserStatistics pulseStat;
	Pulser pulser(1000 / updatePerSecond, Pulser::eSliceCycle, &pulseStat);

	pulser.Start();

	int pollingEventCountPerSec = 0;
	TimeCounter updateCounter(TimeCounterAttribute::TimeOverReset);
	while (true)
	{
		TimeSpan elapsed = pulser.Wait();
		updateCounter.Elapsed += elapsed;

		int processCount = sqlServerDb.PollEvents();
		pollingEventCountPerSec += processCount;

		if (updateCounter.ElapsedSeconds(1))
		{
			Console::Write("SQL Server 이벤트 수: %d\n", pollingEventCountPerSec);
			pollingEventCountPerSec = 0;
		}

		if (GetAsyncKeyState((int)ConsoleKey::X))
		{ 
			break;         // 또는 그냥 break만
		}
	}

	for (int i = 0; i < TCNT; ++i)
	{
		runningThread[i] = false;
		thread[i].Join();
	}

	sqlServerDb.Finalize();
	FinalizeNetLogger();
	FinalizeDefaultLogger();
	FinalizeJCore();
	return 0;
}
