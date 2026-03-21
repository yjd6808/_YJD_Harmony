/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * 통합 Database 클래스 구현
 */

#include "IDatabase.h"

#include "ConnectionPool.h"

USING_NS_JC;
USING_NS_JDB;

//////////////////////////////////////////////////////////////////////////////////////////
int IDatabase::PollEvents()
{
	if (iocp_ == nullptr)
	{
		jc_assert_msg(false, "IOCP가 초기화되지 않았습니다. IOCP를 사용하는 데이터베이스인지 확인해주세요.");
		return -1;
	}
	return iocp_->PollTasks();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IDatabase::Initialize(const DatabaseInfo& _info)
{
	if (initialized_)
	{
		_LogError_("이미 생성된 객체입니다.");
		return false;
	}

	info_ = _info;
	const int connectionPoolSize = info_.connPoolSize_;
	const int maxConnection = info_.maxConnection_;
	const int threadCount = info_.iocpThreadCount_;

	if (connectionPool_ == nullptr)
	{
		connectionPool_ = dbg_new ConnectionPool(
			info_.type_,
			info_.hostName_,
			info_.connPort_,
			info_.accountId_,
			info_.accountPass_,
			info_.dbName_,
			maxConnection);
	}

	// 커넥션 풀 초기화
	if (!connectionPool_->Init(connectionPoolSize))
	{
		JC_DELETE_SAFE(connectionPool_);
		_LogError_("DB 커넥션 풀 초기화 실패");
		return false;
	}

	// 빌더 커넥션 초기화
	// String Escape 하나를 위해서 어쩔수없이 초기화함;
	if (info_.type_ == DatabaseType::dbtMySQL)
	{
		if (!MysqlStatementBuilder::Initialize(info_))
		{
			_LogError_("DB 스테이트먼트 빌더 초기화 실패");
			return false;
		}
	}
	else if (info_.type_ == DatabaseType::dbtSQLServer)
	{
		if (!SqlServerStatementBuilder::Initialize(info_))
		{
			_LogError_("DB 스테이트먼트 빌더 초기화 실패");
			return false;
		}
	}
	
	_LogInfo_("데이터베이스 커넥션 풀(크기: %d) 초기화 [%s:%d]",
		connectionPoolSize,
		info_.hostName_.Source(),
		info_.connPort_);

	if (threadCount > 0)
	{
		iocp_ = dbg_new jnet::IOCP(threadCount);
		iocp_->SetName(info_.name_);
		iocp_->SetBatchSize(info_.iocpBatchSize_);
		iocp_->SetPollingMode(info_.iocpPollingMode_);
		iocp_->SetCompletedCallback(QueryCompletedCallbackFunctor{ this });
		iocp_->Run();
		_LogInfo_("%s %s 실행완료 (쓰레드 수: %d)", info_.name_.Source(), jnet::IOCP::TypeName(), threadCount);
	}
	else
	{
		_LogInfo_("%s 쓰레딩 사용안함. (QueryAsync 사용 불가능.)", info_.name_.Source());
	}
	initialized_ = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IDatabase::Finalize()
{
	if (initialized_ == false)
		return;

	_LogInfo_("%s 파괴시작", info_.name_.Source());
	initialized_ = false;

	if (iocp_)
	{
		iocp_->Join();
		_LogInfo_("%s %s 쪼인완료", info_.name_.Source(), jnet::IOCP::TypeName());

		iocp_->Destroy();
		_LogInfo_("%s %s 파괴완료", info_.name_.Source(), jnet::IOCP::TypeName());
	}

	// iocp가 먼저 소멸되어야함. 태스크가 커넥션을 참조하기 때문.
	JC_DELETE_SAFE(iocp_);
	JC_DELETE_SAFE(connectionPool_);

	_LogInfo_("%s 커넥션 풀 파괴완료", info_.name_.Source());

	if (info_.type_ == DatabaseType::dbtMySQL)
	{
		MysqlStatementBuilder::Finalize();
	}
	else if (info_.type_ == DatabaseType::dbtSQLServer)
	{
		SqlServerStatementBuilder::Finalize();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
IQueryPtr IDatabase::Query(const Ptmt& _ptmt) const
{
	return Query(0, _ptmt);
}

//////////////////////////////////////////////////////////////////////////////////////////
IQueryPtr IDatabase::Query(int _id, const Ptmt& _ptmt) const
{
	if (connectionPool_ == nullptr)
	{
		jc_assert_msg(false, "커넥션 풀이 초기화되지 않았습니다. 데이터베이스가 연결되어있는지 확인해주세요.");
		return nullptr;
	}

	auto pConn = connectionPool_->GetConnection();
	if (pConn == nullptr)
	{
		jc_assert_msg(false, "SqlServerDatabase::Query() 커넥션 풀에서 가져오기 실패");
		return nullptr;
	}

	AutoReleaseConnection autoRelease(pConn, connectionPool_);

	IQueryPtr pQuery = CreateQuery(pConn, _ptmt);

	if (pQuery == nullptr)
	{
		jc_assert_msg(false, "SqlServerDatabase::Query() 쿼리문 파싱 실패");
		return nullptr;
	}
	pQuery->SetId(_id);
	pQuery->Execute();

	if (fnQueryCompleted_)
	{
		fnQueryCompleted_(*pQuery);
	}
	return pQuery;
}

//////////////////////////////////////////////////////////////////////////////////////////
IQueryTaskPtr IDatabase::QueryAsync(const Ptmt& _ptmt) const
{
	return QueryAsyncInternal(0, _ptmt);
}

//////////////////////////////////////////////////////////////////////////////////////////
IQueryTaskPtr IDatabase::QueryAsync(int _id, const Ptmt& _ptmt) const
{
	return QueryAsyncInternal(_id, _ptmt);
}

//////////////////////////////////////////////////////////////////////////////////////////
IQueryTaskPtr IDatabase::QueryAsyncInternal(int _id, const Ptmt& _stmt) const
{
	if (iocp_ == nullptr)
	{
		jc_assert_msg(false, "IOCP가 초기화되지 않았습니다. 쓰레딩이 필요한 기능을 사용하려면 IOCP를 초기화해주세요.");
		return nullptr;
	}
	if (connectionPool_ == nullptr)
	{
		jc_assert_msg(false, "커넥션 풀이 초기화되지 않았습니다.");
		return nullptr;
	}

	auto pConn = connectionPool_->GetConnection();
	if (pConn == nullptr)
	{
		jc_assert_msg(false, "QueryAsyncInternal() 커넥션 풀에서 가져오기 실패");
		return nullptr;
	}

	IQueryPtr pQuery = CreateQuery(pConn, _stmt);
	if (pQuery == nullptr)
	{
		connectionPool_->ReleaseConnection(pConn);
		jc_assert_msg(false, "QueryAsyncInternal() 쿼리문 파싱 실패");
		return nullptr;
	}
	pQuery->SetId(_id);

	const _u32 failedCode = GetQueryFailedErrorCode();
	const auto& taskFunc = [failedCode](IQueryTask::TResult& _result)
	{
		_result.success_ = _result.value_->Execute();
		_result.errorCode_ = !_result.success_ ? failedCode : 0;
	};

	const auto& finallyFunc = [this, pConn](IQueryTask::TResult& _result)
	{
		connectionPool_->ReleaseConnection(pConn);
	};

	auto pTask = IQueryTask::Create(iocp_, taskFunc, finallyFunc, pQuery);
	pTask->SetOwnerId(id_);
	pTask->SetType(GetTaskType());
	pTask->Start();
	return pTask;
}

//////////////////////////////////////////////////////////////////////////////////////////
void QueryCompletedCallbackFunctor::operator()(jnet::IOCPTaskAbstract* _pTask)
{
	if (pDb_->fnQueryCompleted_)
	{
		auto pTask = (jnet::IOCPTask<jdb::IQueryPtr>*)_pTask;
		jdb::IQuery* pQuery = pTask->GetResult().value_.GetPtr();
		if (pQuery != nullptr)
		{
			pDb_->fnQueryCompleted_(*pQuery);
		}
	}
	
}
