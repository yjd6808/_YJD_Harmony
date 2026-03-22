// $(Root)/Props/base.props ForceInclude 파일 형식 맞추기 위해 넣음
#pragma once

#include "jdb/Structure.h"

enum JDB_STMT
{
	stmt1,
	stmt2,
	stmtMax,
};

extern jdb::StmtTemplate _stmtList[stmtMax];
#define JDB_DEF_STMT(x)	#x


