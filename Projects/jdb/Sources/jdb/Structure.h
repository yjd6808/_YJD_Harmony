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
	jc::String name_;
	jc::String hostName_;
	_u16 connPort_ = 0;
	jc::String accountId_;
	jc::String accountPass_;
	jc::String schemaName_;
	int maxConnection_ = 0;
	int connPoolSize_ = 0;
	int iocpThreadCount_ = 0;
};

NS_END