#include <jdb/Core.h>

#include "jc/Threading/Pulser.h"
#include "jdb/SQLServer/SqlServerDatabase.h"

USING_NS_JC;

int main(int _argc, char** _argv) 
{
	InitializeJCore(_argc, _argv);
	InitializeDefaultLogger();
	InitializeNetLogger();

	jdb::DatabaseInfo info;
	info.type_ = jdb::DatabaseType::SQLServer;
	info.name_ = "게임DB";
	info.hostName_ = "127.0.0.1";
	info.connPort_ = 1433;
	info.accountId_ = "sa";
	info.accountPass_ = "1234";
	info.dbName_ = "steinsgate";
	info.maxConnection_ = 100;
	info.connPoolSize_ = 10;
	info.iocpThreadCount_ = 12;
	info.iocpPollingMode_ = true;

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
				jc::String s1 = _query.GetString(0);
				jc::String s2 = _query.GetString(1);

				_query.Next();
				s1 = _query.GetString(0);
				s2 = _query.GetString(1);

			}
			break;
		case QID_SELECT_2:
			{
				
				_u32 fieldCount = _query.GetFieldCount();

				// c_uid, c_float, c_double, c_s8, c_s16, c_s32, c_s64, c_u8, c_u16, c_u32, c_u64, c_datetime FROM dbo.t_test
				do
				{
					jc::String s1 = _query.GetString(0);
					jc::String s2 = _query.GetString(1);
					jc::String s3 = _query.GetString(2);
					jc::String s4 = _query.GetString(3);
					jc::String s5 = _query.GetString(4);
					jc::String s6 = _query.GetString(5);
					jc::String s7 = _query.GetString(6);
					jc::String s8 = _query.GetString(7);
					jc::String s9 = _query.GetString(8);
					jc::String s10 = _query.GetString(9);
					jc::String s11 = _query.GetString(10);
					jc::String s12 = _query.GetString(11);
					jc::String s13 = _query.GetString(11);
					jc::String s14 = _query.GetString(11);
					jc::String s15 = _query.GetString(11);

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
		db.QueryAsync(QID_INSERT_1, "INSERT INTO t_account (c_account_id, c_account_pass) VALUES({0}, {1}); ", id, pass);
		 
		 // jc_assert_msg(rowCount == 1, "영향받은 행 갯수가 1이 아닙니다. rowCount: %u", rowCount);

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
				"c_datetime)"
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
			"SYSDATETIME());";
		db.QueryAsync(QID_INSERT_2, text, "abcd", "efgh", "00");
	}

	// ------------------------------------------
	// delete 테스트
	{
		db.QueryAsync(QID_DELETE_1, "DELETE FROM t_account WHERE c_account_id = {0}", StringUtil::Format("test%d", x));
	}





	// ------------------------------------------
	// select 테스트

	db.QueryAsync(QID_SELECT_2, "SELECT c_uid, c_varchar, c_char, c_nvarchar, c_float, c_double, c_s8, c_s16, c_s32, c_s64, c_u8, c_u16, c_u32, c_u64, c_datetime FROM dbo.t_test");

	constexpr int TCNT = 2;
	bool runningThread[TCNT]{};
	Thread thread[TCNT];
	{
		for (int i = 0; i < TCNT; ++i)
		{
			thread[i].Start([i, &db, &runningThread](void* _param)
			{
				runningThread[i] = true;
				while (runningThread[i])
				{
					// auto pQuery = db.QueryAsync(QID_SELECT_1, "select * from t_account");
					// Sleep(1);
				}
			});
		}

		//auto pQuery = db.QueryAsync(QID_SELECT_1, "select * from t_account");
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

		int processCount = db.PollEvents();
		pollingEventCountPerSec += processCount;

		if (updateCounter.ElapsedSeconds(1))
		{
			Console::Write("SQL Server 이벤트 수: %d, 수행 대기중 오버랩 수: %d\n", pollingEventCountPerSec, db.GetPendingQueryCount());
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

	db.Finalize();
	FinalizeNetLogger();
	FinalizeDefaultLogger();
	FinalizeJCore();
	return 0;
}
