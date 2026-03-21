/*
 * 작성자: 윤정도
 */
#pragma once

#include "jc/Primitives/String.h"
#include "jc/Type.h"

#include "jdb/Namespace.h"

NS_JDB_BEGIN

enum DatabaseType
{
	dbtNone,
	dbtMySQL,
	dbtSQLServer,
};

struct DatabaseInfo
{
	DatabaseType type_ = DatabaseType::dbtMySQL;
	int id_ = -1;				// DB 구분용 ID (예: 게임DB, 로그DB 등) 데이터 파일/헤더파일 모두 정의하여 사용하여 고유 ID로 사용하는 용도

	jc::String name_;			// 게임DB
	jc::String hostName_;		// 127.0.0.1
	_u16 connPort_ = 0;			// MySQL: 3306, SQLServer: 1433
	jc::String accountId_;		// MySQL: root, SQLServer: sa
	jc::String accountPass_;	// 1234
	jc::String dbName_;			// steinsgate (MySQL의 schemaName과 동일)
	int maxConnection_ = 0;
	int connPoolSize_ = 0;
	int iocpThreadCount_ = 0;
	bool iocpPollingMode_ = false;
	int iocpBatchSize_ = 16; // 한번에 처리가능한 최대 
};

NS_END