/*
 * 작성자: 윤정도
 * 생성일: 10/9/2023 4:29:06 PM
 * =====================
 *
 */


#include "Pch.h"

#include <TF/Server/Const.h>
#include <TF/Server/Contents/Room.h>
#include <TF/Server/Contents/Character.h>

void InitializeCore() {
	Player::InitPool(
		Const::AccessibleObject::InitCapacity::Player,
		Const::AccessibleObject::InitSize::Player,
		Const::AccessibleObject::StartId::Player
	);

	Room::InitPool(
		Const::AccessibleObject::InitCapacity::Room,
		Const::AccessibleObject::InitSize::Room,
		Const::AccessibleObject::StartId::Room
	);

	Character::InitPool(
		Const::AccessibleObject::InitCapacity::Character,
		Const::AccessibleObject::InitSize::Character,
		Const::AccessibleObject::StartId::Character
	);

	DatabaseInfo* pDatabaseInfo = dbg_new DatabaseInfo;
	pDatabaseInfo->HostName = "127.0.0.1";
	pDatabaseInfo->ConnectionPort = 3307;
	pDatabaseInfo->Name = "게임 DB";
	pDatabaseInfo->Use = true;
	pDatabaseInfo->ConnectionPoolSize = 10;
	pDatabaseInfo->AccountId = "root";
	pDatabaseInfo->AccountPass = "1234";
	pDatabaseInfo->SchemaName = "tankfighter";
	pDatabaseInfo->MaxConnection = -1;	// 안쓰임
	pDatabaseInfo->IocpThreadCount = 4;

	Core::GameDB = dbg_new MysqlDatabase(pDatabaseInfo);
	Core::GameDB->Initialize();

	Core::World = dbg_new World;
	Core::World->Initialize();

	Core::NetCore = dbg_new NetCore;
	Core::NetCore->Initialize();


}

void FinalizeCore() {
	Core::World->Finalize();

	JCORE_DELETE_SAFE(Core::NetCore);
	JCORE_DELETE_SAFE(Core::World);
	JCORE_DELETE_SAFE(Core::GameDB);

	Character::FreeAllObjects();
	Room::FreeAllObjects();
	Player::FreeAllObjects();
}