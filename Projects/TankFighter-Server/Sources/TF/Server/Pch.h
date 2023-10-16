/*
 * 작성자: 윤정도
 * 생성일: 10/9/2023 4:28:58 PM
 * =====================
 *
 */


#pragma once

#include <TF/Common/PchBase.h>

#include <TF/Server/NetCore.h>
#include <TF/Server/Database/MysqlDatabase.h>
#include <TF/Server/Contents/World.h>

#include <mysql.h>

namespace Core
{
	inline ::NetCore* NetCore;
	inline ::MysqlDatabase* GameDB;
	inline ::World* World;
}

void InitializeCore();
void FinalizeCore();