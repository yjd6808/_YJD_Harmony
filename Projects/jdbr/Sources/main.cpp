#include <jdb/Core.h>

#include "jc/Threading/Pulser.h"
#include "jdb/SQLServer/SqlServerDatabase.h"

USING_NS_JC;

#define MULTITHREAD_TEST	1

#if  MULTITHREAD_TEST > 0
#define Query	QueryAsync
#else
#define Query	Query
#endif


int main(int _argc, char** _argv) 
{
	InitializeJCore(_argc, _argv);
	InitializeDefaultLogger();
	InitializeNetLogger();

	jdb::DatabaseInfo info;
	info.type_ = jdb::DatabaseType::dbtSQLServer;
	info.name_ = "게임DB";
	info.hostName_ = "127.0.0.1";
	info.connPort_ = 1433;
	info.accountId_ = "sa";
	info.accountPass_ = "1234";
	info.dbName_ = "steinsgate";
	info.maxConnection_ = 100;
	info.connPoolSize_ = 100;
	info.iocpThreadCount_ = MULTITHREAD_TEST ? 8 : 0;
	info.iocpPollingMode_ = true;
	info.iocpBatchSize_ = 16;

	jdb::IDatabasePtr pDb = jc::MakeShared<jdb::SqlServerDatabase>();
	jdb::IDatabase& db = *pDb.GetPtr();
	if (db.Initialize(info) == false)
	{
		FinalizeNetLogger();
		FinalizeDefaultLogger();
		FinalizeJCore();
		return -1;
	}

	static constexpr int QID_SELECT_1 = 100;
	static constexpr int QID_SELECT_2 = 101; // t_test
	static constexpr int QID_INSERT_1 = 200;
	static constexpr int QID_INSERT_2 = 201; // t_test
	static constexpr int QID_DELETE_1 = 300;

	// jdb::SqlServerStatementBuilder::Build("abcd {0} {1} {0}", 10, 2);

	db.SetQueryCompletedCallback([](jdb::IQuery& _query)
	{
		_u32 affectedRowCount = 0;
		switch (_query.GetId())
		{
		case QID_SELECT_1:
			{

			}
			break;
		case QID_SELECT_2:
			{
				
				_u32 fieldCount = _query.GetColCount();

				// c_uid, c_float, c_double, c_s8, c_s16, c_s32, c_s64, c_u8, c_u16, c_u32, c_u64, c_datetime FROM dbo.t_test
				do
				{
					int col = 0;
					int c1 = _query.GetS32(col++);
					jc::String c1_1 = _query.GetString(col++);
					jc::String c1_2 = _query.GetString(col++);
					jc::String c1_3 = _query.GetString(col++);
					_f32 c2 = _query.GetFloat(col++);
					_f64 c3 = _query.GetDouble(col++);
					_s8 c4 = _query.GetS8(col++);
					_s16 c5 = _query.GetS16(col++);
					_s32 c6 = _query.GetS32(col++);
					_s64 c7 = _query.GetS64(col++);
					_u8 c8 = _query.GetU8(col++);
					_u16 c9 = _query.GetU16(col++);
					_u32 c10 = _query.GetU32(col++);
					_u64 c11 = _query.GetU64(col++);
					jc::DateTime c12 = _query.GetDateTime(col++);
					jc::Date c13 = _query.GetDate(col++);
					jc::Time c14 = _query.GetTime(col++);

				} while (_query.Next());

				affectedRowCount = _query.GetRowCount(); // 끝까지 읽었으니 여기서 RowCount가 유효하다.
				
			}
			break;
		case QID_INSERT_1:
			{
				affectedRowCount = _query.GetRowCount();
			}
			break;
		case QID_DELETE_1:
			{
				affectedRowCount = _query.GetRowCount();
			}
			break;
		}

		return;
	});

	// ------------------------------------------
	// insert 테스트
	int x = 5;
	{

		
		String id = StringUtil::Format("test%d", x);
		String pass = StringUtil::Format("pass%d", x);
		db.Query(QID_INSERT_1, "INSERT INTO t_account (c_account_id, c_account_pass) VALUES({0}, {1}); ", id, pass);
		 
		 // jc_assert_msg(rowCount == 1, "영향받은 행 갯수가 1이 아닙니다. rowCount: %u", rowCount);
		// db.Query("delete from t_test");
		jc::String text = 
			"INSERT INTO dbo.t_test ("
				"c_float,"
				"c_double,"
				"c_varchar,"
				"c_char,"
				"c_nvarchar,"
				"c_s8,"
				"c_s16,"
				"c_s32,"
				"c_s64,"
				"c_u8,"
				"c_u16,"
				"c_u32,"
				"c_u64,"
				"c_datetime,"
				"c_date,"
				"c_time)"
			"VALUES ("
				"1.5,"
				"3.141592,"
				"{0},"
				"{1},"
				"{2},"
				"-10,"
				"-200,"
				"-30000,"
				"-9000000000,"
				"200,"
				"50000,"
				"3000000000,"
				"1000000000000000000,"
				"SYSDATETIME(),"
				"SYSDATETIME(),"
			"SYSDATETIME());";
		//db.Query(QID_INSERT_2, text, "abcd한글", "efgh한글", "00한글 사랑");
		
	}

	// ------------------------------------------
	// delete 테스트
	{
		db.Query(QID_DELETE_1, "DELETE FROM t_account WHERE c_account_id = {0}", StringUtil::Format("test%d", x));
	}

	jdb::Ptmt ptmt = jdb::Ptmt::Build(jdb::dbtSQLServer, "{0}{1}{0}", 10, 20);



	// ------------------------------------------
	// select 테스트

	db.Query(QID_SELECT_2, "SELECT c_uid, c_varchar, c_char, c_nvarchar, c_float, c_double, c_s8, c_s16, c_s32, c_s64, c_u8, c_u16, c_u32, c_u64, c_datetime, c_date, c_time FROM dbo.t_test");
	


	constexpr int TCNT = 20;
	jc::Atomic<int> counter = 0;
	bool runningThread[TCNT]{};
	Thread thread[TCNT];
	for (int i = 0; i < TCNT; ++i)
	{
		thread[i].Start([i, &db, &runningThread, &counter](void* _param)
		{
			runningThread[i] = true;
			while (runningThread[i])
			{
				auto pQuery = db.Query(QID_SELECT_1, "select * from t_account");
				++counter;
				Sleep(1);

				// Sleep을 걸지 않을 경우.
				// MULTITHREAD_TEST가 1일 경우 쿼리가 생성되는 속도가 너무 빨라서
				// IOCP의 커널 큐에 초당 들어오는 아이템 수가 전체 IOCPWorker의 초당 처리량을 초과하기 때문에
				// 지속적으로 CreateConnection을 수행하게 되는 현상이 발생할 수 있다.
				// 때문에 Sleep(1)을 걸어주도록 한다.
			}
		});
	}

	// pQuery 가드, db Finalize된 이후에 호출되는건 ub임
	// db.Query(QID_SELECT_1, "select * from t_account");
	

	Console::Write("%s\n", "x키 입력시 종료");

	int updatePerSecond = 10;
	PulserStatistics pulseStat;
	Pulser pulser(1000 / updatePerSecond, Pulser::eSliceCycle, &pulseStat);

	pulser.Start();

	int pollingEventCountPerSec = 0;
	TimeCounter updateCounter(TimeCounterAttribute::TimeOverReset);
	DateTime prevTime = DateTime::Now();

	constexpr int USE_PULSER = 1;
	while (true)
	{
		DateTime now = DateTime::Now();
		TimeSpan elapsed;
		if constexpr (USE_PULSER)
		{
			elapsed = pulser.Wait();
		}
		else
		{
			elapsed = now.Diff(prevTime);	
		}
		
		updateCounter.Elapsed += elapsed;

		int processCount = 0;
		if constexpr (MULTITHREAD_TEST > 0)
		{
			if (db.IsPollingMode())
			{
				processCount = db.PollEvents();
			}
		}
		else
		{
			int prev = counter.Exchange(0);
			++processCount = prev;
		}
		
		pollingEventCountPerSec += processCount;

		if (updateCounter.ElapsedSeconds(1))
		{
			int usingConnCount = 0;
			int availableConnCount = 0;
			db.GetConnectionPool()->GetConnCount(usingConnCount, availableConnCount);
			Console::Write("SQL Server 이벤트 수: %d, 수행 대기중 오버랩 수: %d, (쿼리 생성 수: %d, 사용 중 연결: %d, 비사용 중 연결: %d)\n", pollingEventCountPerSec, db.GetPendingQueryCount(), counter, usingConnCount, availableConnCount);
			pollingEventCountPerSec = 0;
		}

		if (GetAsyncKeyState((int)ConsoleKey::X))
		{ 
			break;         // 또는 그냥 break만
		}
		prevTime = now;
	}

	for (int i = 0; i < TCNT; ++i)
	{
		runningThread[i] = false;
		thread[i].Join();
	}

	db.Finalize();
	FinalizeNetLogger();
	FinalizeDefaultLogger();
	FinalizeJCore();
	return 0;
}
