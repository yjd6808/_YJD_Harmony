//
//
//
//#include "DFDataBaseManager.h"
//#include "DFMysqlQuery.h"
//#include "../DFUtil/DFUtil.hpp"
//#include "../DFPacketManager/DFPacketMap.h"
//#include "MysqlConnectionPool.h"
//#include <boost/algorithm/string.hpp>
//
//using namespace std;
//
//DFDataBaseManager* DFDataBaseManager::s_instance = nullptr;
//
//DFDataBaseManager::DFDataBaseManager()
//{
//}
//
//DFDataBaseManager::~DFDataBaseManager()
//{
//}
//
//DFDataBaseManager * DFDataBaseManager::getInstance()
//{
//	if (s_instance == nullptr)
//	{
//		s_instance = new DFDataBaseManager;
//
//	}
//
//	return s_instance;
//}
//
//bool DFDataBaseManager::connect()
//{
//	MYSQL		mysql;
//
//	mysql_init(&mysql);
//	if (!mysql_real_connect(&mysql, DB_HOST, DB_ID, DB_PASS, DB_NAME, DB_PORT, (char *)NULL, 0)) 
//	{
//		printf("%s\n", mysql_error(&mysql));
//		return false;
//	}
//
//	Console::WriteLine(ConsoleColor::GREEN, "[Info] Loaded Database Information [localhost/%d]", DB_PORT);
//	return true;
//}
//
//void DFDataBaseManager::_register(DFTCPClient* client, std::string id, std::string pw)
//{
//
//	auto connection = DFMysqlPool.GetConnection();
//
//	string clientip = client->getIPAddress();
//
//	MySQLQuery *sqlQuery = new MySQLQuery(connection, "SELECT user_id, user_pass, ip FROM accounts WHERE ip = ? OR user_id = ?");
//	sqlQuery->setString(1, clientip);
//	sqlQuery->setString(2, id);
//	sqlQuery->ExecuteQuery();
//	
//
//	if (sqlQuery->GetResultRowCount() >= 1)
//	{
//		string user_id = sqlQuery->getString(1, "user_id");
//		string ip = sqlQuery->getString(1, "ip");
//		DFPacketRegister reg;
//		if (strcmp(id.c_str(), user_id.c_str()) == 0)
//		{
//			reg.mRet = DFPacketRegister::RESULT::FAIL_ID_EXIST;
//			client->sendRequest(&reg);
//		}
//		else if (strcmp(clientip.c_str(), ip.c_str()) == 0)
//		{
//			reg.mRet = DFPacketRegister::RESULT::FAIL_IP_EXIST;
//			client->sendRequest(&reg);
//		}
//		else
//		{
//			reg.mRet = DFPacketRegister::RESULT::NONE;
//			client->sendRequest(&reg);
//		}
//		delete sqlQuery;
//	}
//	else
//	{
//		MySQLQuery *sqlQuery2 = new MySQLQuery(connection, "insert into accounts (user_id, user_pass, ip) values (?, ?, ?)");
//		sqlQuery2->setString(1, id);
//		sqlQuery2->setString(2, pw);
//		sqlQuery2->setString(3, clientip);
//		sqlQuery2->ExecuteQuery();
//
//		DFPacketRegister reg;
//		reg.mRet = DFPacketRegister::RESULT::SUCCESS;
//		strcmp(reg.mId, id.c_str());
//		strcmp(reg.mPass, pw.c_str());
//		client->sendRequest(&reg);
//	}
//	DFMysqlPool.ReleaseConnection(connection);
//}
//
//void DFDataBaseManager::_login(DFTCPClient * client, DFPacketLogin * login)
//{
//	auto connection = DFMysqlPool.GetConnection();
//
//	MySQLQuery *sqlQuery = new MySQLQuery(connection, "SELECT id, user_id, user_pass, loggedin, banned FROM accounts WHERE user_id = ? AND user_pass = ?");
//	sqlQuery->setString(1, login->mId);
//	sqlQuery->setString(2, login->mPass);
//	sqlQuery->ExecuteQuery();
//
//	if (sqlQuery->GetResultRowCount() >= 1)
//	{
//		
//		int accountid = sqlQuery->getInt(1, "id");
//		int islogon = sqlQuery->getInt(1, "loggedin");
//		int isbanned = sqlQuery->getInt(1, "banned");
//
//		delete sqlQuery;
//		if (islogon == true)
//			login->mRet = DFPacketLogin::RESULT::ALREADY_LOGIN;
//		else if (isbanned == true)
//			login->mRet = DFPacketLogin::RESULT::FAIL_ID_BANNDED;
//		else
//		{
//			login->mRet = DFPacketLogin::RESULT::SUCCESS;
//
//			MySQLQuery *sqlQuery2 = new MySQLQuery(connection, "UPDATE accounts set loggedin = 1 where user_id = ?");
//			sqlQuery2->setString(1, login->mId);
//			sqlQuery2->ExecuteQuery();
//			delete sqlQuery2;
//
//			MySQLQuery *sqlQuery3 = new MySQLQuery(connection, "SELECT id, job, name, level, gm FROM characters WHERE account_id = ?");
//			sqlQuery3->setInt(1, accountid);
//			sqlQuery3->ExecuteQuery();
//
//			for (int i = 1; i <= sqlQuery3->GetResultRowCount(); i++)
//			{
//				DFPacketCharacterInfo info;
//				info.mPlayerID = sqlQuery3->getInt(i, 0);
//				info.mJob = sqlQuery3->getInt(i, 1);
//				strcpy(info.mNick, sqlQuery3->getString(i, 2).c_str());
//				info.mLevel = sqlQuery3->getInt(i, 3);
//				info.mGM = sqlQuery3->getInt(i, 4);
//				memcpy(&login->mCharactersArray[i-1], &info, sizeof(DFPacketCharacterInfo));
//			}
//			login->mCharacterCount = sqlQuery3->GetResultRowCount();
//			login->mAccountID = accountid;
//			client->mAccountID = accountid;
//		}
//		client->sendRequest(login);
//	
//	}
//	else
//	{
//		login->mRet = DFPacketLogin::RESULT::FAIL_IDPW_WRONG;
//		client->sendRequest(login);
//	}
//	DFMysqlPool.ReleaseConnection(connection);
//}
//
//void DFDataBaseManager::_logout(DFTCPClient* client, DFPacketLogout* logout)
//{
//	auto connection = DFMysqlPool.GetConnection();
//
//
//	MySQLQuery *sqlQuery = new MySQLQuery(connection, "UPDATE accounts set loggedin = 0 where id = ?");
//	sqlQuery->setInt(1, logout->mAccountID);
//	sqlQuery->ExecuteQuery();
//
//	logout->mAccountID = 0;
//	client->mAccountID = 0;
//	client->sendRequest(logout);
//	DFMysqlPool.ReleaseConnection(connection);
//}
//
//void DFDataBaseManager::_logout(int account_od)
//{
//	auto connection = DFMysqlPool.GetConnection();
//
//	MySQLQuery *sqlQuery = new MySQLQuery(connection, "UPDATE accounts set loggedin = 0 where id = ?");
//	sqlQuery->setInt(1, account_od);
//	sqlQuery->ExecuteQuery();
//	DFMysqlPool.ReleaseConnection(connection);
//
//}
//
//void DFDataBaseManager::_duplicationCheck(DFTCPClient * client, DFPacketNickDuplication * dupPacket)
//{
//	auto connection = DFMysqlPool.GetConnection();
//
//	MySQLQuery *sqlQuery = new MySQLQuery(connection, "SELECT name FROM characters WHERE name = ?");
//	sqlQuery->setString(1, dupPacket->mNick);
//	sqlQuery->ExecuteQuery();
//
//	if (sqlQuery->GetResultRowCount() > 0)
//		dupPacket->mRet = DFPacketNickDuplication::RESULT::ALREADY_EXIST;
//	else
//		dupPacket->mRet = DFPacketNickDuplication::RESULT::NO_EXIST;
//	client->sendRequest(dupPacket);
//	DFMysqlPool.ReleaseConnection(connection);
//
//	//insert into characters account_id, name, job values (?, ?, ?)
//}
//
//void DFDataBaseManager::_createCharacter(DFTCPClient * client, DFPacketCreateCharacter * ccPacket)
//{
//	MySQLConnection* connection = new MySQLConnection();
//	connection->Connect(DB_HOST, DB_PORT, DB_ID, DB_PASS, DB_NAME);
//
//	MySQLQuery *sqlQuery = new MySQLQuery(connection, "SELECT name FROM characters WHERE name = ?");
//	sqlQuery->setString(1, ccPacket->mNick);
//	sqlQuery->ExecuteQuery();
//
//	if (sqlQuery->GetResultRowCount() > 0)
//	{
//		ccPacket->mRet = DFPacketCreateCharacter::RESULT::ALREADY_EXIST;
//		delete sqlQuery;
//	}
//	else
//	{
//		delete sqlQuery;
//		
//		MySQLQuery *sqlQuery2= new MySQLQuery(connection, "insert into characters (account_id, name, job) values (?, ?, ?)");
//		sqlQuery2->setInt(1, ccPacket->mAccountID);
//		sqlQuery2->setString(2, ccPacket->mNick);
//		sqlQuery2->setInt(3, 0); //°Å³Ê¸¸
//		sqlQuery2->ExecuteQuery();
//		delete sqlQuery2;
//
//		MySQLQuery *sqlQuery3 = new MySQLQuery(connection, "SELECT id, job, name, level, gm FROM characters WHERE account_id = ?");
//		sqlQuery3->setInt(1, ccPacket->mAccountID);
//		sqlQuery3->ExecuteQuery();
//
//		for (int i = 1; i <= sqlQuery3->GetResultRowCount(); i++)
//		{
//			DFPacketCharacterInfo info;
//			info.mPlayerID = sqlQuery3->getInt(i, 0);
//			info.mJob = sqlQuery3->getInt(i, 1);
//			strcpy(info.mNick, sqlQuery3->getString(i, 2).c_str());
//			info.mLevel = sqlQuery3->getInt(i, 3);
//			info.mGM = sqlQuery3->getInt(i, 4);
//			memcpy(&ccPacket->mCharactersArray[i - 1], &info, sizeof(DFPacketCharacterInfo));
//		}
//		ccPacket->mCharacterCount = sqlQuery3->GetResultRowCount();
//		ccPacket->mRet = DFPacketCreateCharacter::RESULT::SUCCESS;
//	}
//	client->sendRequest(ccPacket);
//	DFMysqlPool.ReleaseConnection(connection);
//}
////
//void DFDataBaseManager::_deleteCharacter(DFTCPClient * client, DFPacketDeleteCharacter * delpacket)
//{
//	MySQLConnection* connection = DFMysqlPool.GetConnection();
//
//	MySQLQuery *sqlQuery = new MySQLQuery(connection, "DELETE FROM characters WHERE (id = ?);");
//	sqlQuery->setInt(1, delpacket->mPlayerID);
//	sqlQuery->ExecuteQuery();
//	delete sqlQuery;
//
//	MySQLQuery *sqlQuery3 = new MySQLQuery(connection, "SELECT id, job, name, level, gm FROM characters WHERE account_id = ?");
//	sqlQuery3->setInt(1, client->getAccountID());
//	sqlQuery3->ExecuteQuery();
//
//	for (int i = 1; i <= sqlQuery3->GetResultRowCount(); i++)
//	{
//		DFPacketCharacterInfo info;
//		info.mPlayerID = sqlQuery3->getInt(i, 0);
//		info.mJob = sqlQuery3->getInt(i, 1);
//		strcpy(info.mNick, sqlQuery3->getString(i, 2).c_str());
//		info.mLevel = sqlQuery3->getInt(i, 3);
//		info.mGM = sqlQuery3->getInt(i, 4);
//		memcpy(&delpacket->mCharactersArray[i - 1], &info, sizeof(DFPacketCharacterInfo));
//	}
//	delpacket->mCharacterCount = sqlQuery3->GetResultRowCount();
//	client->sendRequest(delpacket);
//	DFMysqlPool.ReleaseConnection(connection);
//}
//
//void DFDataBaseManager::_startCharacter(DFTCPClient * client, DFPacketStartCharacter * stPacket)
//{
//	MySQLConnection* connection = DFMysqlPool.GetConnection();
//
//	MySQLQuery *sqlQuery3 = new MySQLQuery(connection, "SELECT level, job, str, `int`, hea, men, hp, mp, maxhp, maxmp, gold, cash, max_inven_weight, name FROM characters WHERE id = ? AND account_id = ?");
//	sqlQuery3->setInt(1, stPacket->mPlayerID);
//	sqlQuery3->setInt(2, stPacket->mAccountID);
//	sqlQuery3->ExecuteQuery();
//
//	int ret = sqlQuery3->GetResultRowCount();
//	if (ret > 0)
//	{
//		stPacket->mLv = sqlQuery3->getInt(1, 0);
//		stPacket->mJob = sqlQuery3->getInt(1, 1);
//		stPacket->mStr = sqlQuery3->getInt(1, 2);
//		stPacket->mInt = sqlQuery3->getInt(1, 3);
//		stPacket->mHea = sqlQuery3->getInt(1, 4);
//		stPacket->mMen = sqlQuery3->getInt(1, 5);
//		stPacket->mHp = sqlQuery3->getInt(1, 6);
//		stPacket->mMp = sqlQuery3->getInt(1, 7);
//		stPacket->mMaxHp = sqlQuery3->getInt(1, 8);
//		stPacket->mMaxMp = sqlQuery3->getInt(1, 9);
//		stPacket->mGold = sqlQuery3->getInt(1, 10);
//		stPacket->mCash = sqlQuery3->getInt(1, 11);
//		stPacket->mMaxInvenWeight = sqlQuery3->getInt(1, 12);
//		strcpy(stPacket->mName, sqlQuery3->getString(1, 13).c_str());
//		client->sendRequest(stPacket);
//	}
//	else 
//	{
//		stPacket->mLv = -1;
//		client->sendRequest(stPacket);
//	}
//	DFMysqlPool.ReleaseConnection(connection);
//}
//
//void DFDataBaseManager::_getShopItems(DFTCPClient * client, DFPacketGetShopItem * gsPacket)
//{
//	MySQLConnection* connection = DFMysqlPool.GetConnection();
//
//	MySQLQuery *sqlQuery = new MySQLQuery(connection, "SELECT item_code, price FROM shopitems WHERE npc_code = ?");
//	sqlQuery->setInt(1, gsPacket->mNpcCode);
//	sqlQuery->ExecuteQuery();
//
//	for (int i = 1; i <= sqlQuery->GetResultRowCount(); i++)
//	{
//		gsPacket->mShopItems[i-1].mItemCode= sqlQuery->getInt(i, 0);
//		gsPacket->mShopItems[i-1].mPrice = sqlQuery->getInt(i, 1);
//	}
//	gsPacket->mItemCount = sqlQuery->GetResultRowCount();
//	client->sendRequest(gsPacket);
//	delete sqlQuery;
//	DFMysqlPool.ReleaseConnection(connection);
//}
////
////void DFDataBaseManager::_buyShopItems(DFTCPClient * client, DFPacketBuyShopItem * gsPacket)
////{
////	MySQLConnection* connection = new MySQLConnection();
////	connection->Connect(DB_HOST, DB_PORT, DB_ID, DB_PASS, DB_NAME);
////
////	MySQLQuery *sqlQuery = new MySQLQuery(connection, "insert into inventoryitems (character_id, account_id, item_code, position, quantity, isequipment) values (?, ?, ?, ?, ?, ?)");
////	sqlQuery->setInt(1, gsPacket->mPlayerID);
////	sqlQuery->setInt(2, gsPacket->mAccountID);
////	sqlQuery->setInt(3, gsPacket->mItemCode);
////	sqlQuery->setInt(4, gsPacket->mPosition);
////	sqlQuery->setInt(5, gsPacket->mItemQuantity);
////	sqlQuery->setInt(6, gsPacket->mIsEquipment);
////	sqlQuery->ExecuteQuery();
////	delete sqlQuery;
////
////	
////
////	if (gsPacket->mIsEquipment)
////	{
////		MySQLQuery *sqlQuery2 = new MySQLQuery(connection, "SELECT id FROM inventoryitems WHERE position = ? AND account_id = ? AND character_id = ?");
////		sqlQuery2->setInt(1, gsPacket->mPosition);
////		sqlQuery2->setInt(2, gsPacket->mAccountID);
////		sqlQuery2->setInt(3, gsPacket->mPlayerID);
////		sqlQuery2->ExecuteQuery();
////
////		short rowCount = sqlQuery2->GetResultRowCount();
////		int inventory_id = sqlQuery2->getInt(1, 0);
////
////		delete sqlQuery2;
////		if (rowCount >= 1)
////		{
////			MySQLQuery *sqlQuery3 = new MySQLQuery(connection, "insert into  (inventory_id, enhancecount, str, int, hea, men, atk, atm, mtk, mtm, rf, rw, rl, rd, ef, ew, el, ed, as, cs, hp, mp, ispacked) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
////			sqlQuery3->setInt(1, inventory_id);
////			sqlQuery3->setInt(2, gsPacket->mEquipmentData.mEnhanceCount);
////			sqlQuery3->setInt(3, gsPacket->mEquipmentData.mStr);
////			sqlQuery3->setInt(4, gsPacket->mEquipmentData.mInt);
////			sqlQuery3->setInt(5, gsPacket->mEquipmentData.mHea);
////			sqlQuery3->setInt(6, gsPacket->mEquipmentData.mMen);
////			sqlQuery3->setInt(7, gsPacket->mEquipmentData.mAtk);
////			sqlQuery3->setInt(8, gsPacket->mEquipmentData.mMtk);
////			sqlQuery3->setInt(9, gsPacket->mEquipmentData.mAtm);
////			sqlQuery3->setInt(10, gsPacket->mEquipmentData.mMtm);
////			sqlQuery3->setInt(11, gsPacket->mEquipmentData.mRf);
////			sqlQuery3->setInt(12, gsPacket->mEquipmentData.mRw);
////			sqlQuery3->setInt(13, gsPacket->mEquipmentData.mRl);
////			sqlQuery3->setInt(14, gsPacket->mEquipmentData.mRd);
////			sqlQuery3->setInt(15, gsPacket->mEquipmentData.mEf);
////			sqlQuery3->setInt(16, gsPacket->mEquipmentData.mEw);
////			sqlQuery3->setInt(17, gsPacket->mEquipmentData.mEl);
////			sqlQuery3->setInt(18, gsPacket->mEquipmentData.mEd);
////			sqlQuery3->setInt(19, gsPacket->mEquipmentData.mAs);
////			sqlQuery3->setInt(20, gsPacket->mEquipmentData.mCs);
////			sqlQuery3->setInt(21, gsPacket->mEquipmentData.mHp);
////			sqlQuery3->setInt(22, gsPacket->mEquipmentData.mMp);
////			sqlQuery3->setInt(23, gsPacket->mEquipmentData.mPacked);
////			sqlQuery3->ExecuteQuery();
////			delete sqlQuery3;
////		}
////	}
////
////
////	MySQLQuery *sqlQuery4 = new MySQLQuery(connection, "UPDATE characters set gold = gold - ? where character_id = ?");
////	sqlQuery4->setInt(1, gsPacket->mPrice);
////	sqlQuery4->setInt(2, gsPacket->mPlayerID);
////	delete sqlQuery4;
////
////
////	connection->Disconnect();
////}
//
