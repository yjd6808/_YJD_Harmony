#include "Core.h"
#include "SqlServerDatabase.h"

#include "SqlServerConnectionPool.h"
#include "SqlServerStatementBuilder.h"

USING_NS_JC;
USING_NS_JNET;
USING_NS_STD;
USING_NS_JDB;

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerDatabase::SqlServerDatabase()
: iocp_(nullptr)
, connectionPool_(nullptr)
, initialized_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerDatabase::~SqlServerDatabase()
{
	if (initialized_)
	{
		Finalize();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerDatabase::Initialize(const SqlServerDatabaseInfo& _info)
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
		connectionPool_ = dbg_new SqlServerConnectionPool(
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
		_LogError_("SQLServer 커넥션 풀 초기화 실패");
		return false;
	}

	// 빌더 초기화
	if (!SqlServerStatementBuilder::Initialize(info_))
	{
		_LogError_("SQLServer 스테이트먼트 빌더 초기화 실패");
		return false;
	}

	_LogInfo_("SQLServer 커넥션 풀(크기: %d) 초기화 [%s:%d]",
	          connectionPoolSize,
	          info_.hostName_.Source(),
	          info_.connPort_
	);

	iocp_ = dbg_new IOCP(threadCount);
	iocp_->SetName(info_.name_);
	iocp_->Run();
	initialized_ = true;
	_LogInfo_("%s %s 실행완료 (쓰레드 수: %d)", info_.name_.Source(), IOCP::TypeName(), threadCount);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerDatabase::Finalize()
{
	if (initialized_ == false)
		return;

	_LogInfo_("%s 파괴시작", info_.name_.Source());

	iocp_->Join();
	_LogInfo_("%s %s 쪼인완료", info_.name_.Source(), IOCP::TypeName());

	iocp_->Destroy();
	_LogInfo_("%s %s 파괴완료", info_.name_.Source(), IOCP::TypeName());

	// 커넥션 풀을 파괴하기 전에 iocp를 우선적으로 파괴해야한다.
	JC_DELETE_SAFE(iocp_);	
	JC_DELETE_SAFE(connectionPool_);

	_LogInfo_("%s %s 파괴완료", info_.name_.Source(), SqlServerConnectionPool::TypeName());
	
	SqlServerStatementBuilder::Finalize();
	initialized_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerDatabase::SetListener(const jnet::IOCPTaskListenerPtr& _pListener)
{
	iocp_->SetListener(_pListener);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerDatabase::SetListener(jnet::IOCPTaskListenerPtr&& _pListener)
{
	iocp_->SetListener(jc::Move(_pListener));
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerDatabase::SetOnTaskCompletedCallback(const jnet::IOCPTaskListener::FnOnTaskCompleted& _fn)
{
	jnet::IOCPTaskListenerPtr pListener = jc::MakeShared<jnet::IOCPTaskListener>();
	pListener->SetTaskCompletedCallback(_fn);
	iocp_->SetListener(pListener);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerDatabase::SetOnTaskCompletedCallback(jnet::IOCPTaskListener::FnOnTaskCompleted&& _fn)
{
	jnet::IOCPTaskListenerPtr pListener = jc::MakeShared<jnet::IOCPTaskListener>();
	pListener->SetTaskCompletedCallback(jc::Move(_fn));
	iocp_->SetListener(pListener);
}

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerConnection* SqlServerDatabase::BeginTransaction()
{
	if (connectionPool_ == nullptr)
	{
		jc_assert_msg(false, "커넥션 풀이 초기화되지 않았습니다.");
		return nullptr;
	}

	auto pConn = connectionPool_->GetConnection();
	if (pConn == nullptr)
	{
		jc_assert_msg(false, "SqlServerDatabase::BeginTransaction() 커넥션 풀에서 가져오기 실패");
		return nullptr;
	}

	if (!pConn->BeginTransaction())
	{
		connectionPool_->ReleaseConnection(pConn);
		return nullptr;
	}

	return pConn;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerDatabase::CommitTransaction(SqlServerConnection* _pConn)
{
	if (_pConn == nullptr)
	{
		_LogError_("CommitTransaction() 실패 : 커넥션이 NULL입니다.");
		return false;
	}

	bool result = _pConn->Commit();
	connectionPool_->ReleaseConnection(_pConn);
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerDatabase::RollbackTransaction(SqlServerConnection* _pConn)
{
	if (_pConn == nullptr)
	{
		_LogError_("RollbackTransaction() 실패 : 커넥션이 NULL입니다.");
		return false;
	}

	bool result = _pConn->Rollback();
	connectionPool_->ReleaseConnection(_pConn);
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SqlServerDatabase::PollEvents()
{
	return iocp_->PollTasks();
}

