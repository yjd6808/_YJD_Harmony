/*
 * 작성자: 윤정도
 */
#pragma once

#include <jc/Primitives/String.h>
#include <jc/Type.h>

#include <jdb/Namespace.h>

NS_JDB_BEGIN

struct MysqlDatabaseInfo
{
	jc::String name_;			// 게임DB
	jc::String hostName_;		// 127.0..1
	_u16 connPort_ = 0;			// 3306
	jc::String accountId_;		// root
	jc::String accountPass_;	// 1234
	jc::String schemaName_;		// steinsgate
	int maxConnection_ = 0;
	int connPoolSize_ = 0;
	int iocpThreadCount_ = 0;
};

// 서버 이름(S): 127.0.0.1,1433과
// 인증(A): SQL Server 인증
// 	로그인(L): sa
// 	암호(P): 1234
struct SqlServerDatabaseInfo
{
	jc::String name_;		// 게임DB
	jc::String hostName_;	// 127.0..1
	_u16 connPort_ = 0;		// 1433
	jc::String accountId_;	// sa
	jc::String accountPass_;// 1234
	jc::String dbName_;		// steinsgate
	int maxConnection_ = 0;
	int connPoolSize_ = 0;
	int iocpThreadCount_ = 0;
};

NS_END