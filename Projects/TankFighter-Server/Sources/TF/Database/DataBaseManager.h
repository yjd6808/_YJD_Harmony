/*
	생성일 : 2019/03/06
	작성자 : 윤정도
 */


#ifndef _DFDATABASE_H__
#define _DFDATABASE_H__

#define DB_HOST "127.0.0.1"
#define DB_ID	"root"
#define DB_PASS	"1234"
#define DB_NAME	"dnfmemory"
#define DB_PORT 3307


//#include "../DFTCPClient/DFTCPClient.h"
//#include <string>
//
//class DFDataBaseManager
//{
//public:
//	static DFDataBaseManager* s_instance;
//private:
//	DFDataBaseManager();
//	~DFDataBaseManager();
//public:
//	static DFDataBaseManager* getInstance();
//	bool connect();
//
//	void _register(DFTCPClient* client, std::string id, std::string pw);
//	void _login(DFTCPClient* client, DFPacketLogin* login);
//	void _logout(DFTCPClient* client, DFPacketLogout* logout);
//	void _logout(int account_od);
//	void _duplicationCheck(DFTCPClient* client, DFPacketNickDuplication* dupPacket);
//	void _createCharacter(DFTCPClient* client, DFPacketCreateCharacter* dupPacket);
//	void _deleteCharacter(DFTCPClient* client, DFPacketDeleteCharacter* dupPacket);
//	void _startCharacter(DFTCPClient* client, DFPacketStartCharacter* stPacket);
//	void _getShopItems(DFTCPClient* client, DFPacketGetShopItem* gsPacket);
//	//void _buyShopItems(DFTCPClient* client, DFPacketBuyShopItem* gsPacket);
//};
#endif // _DFDATABASE_H__

