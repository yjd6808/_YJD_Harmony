#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Research/TcpClientNetGroup.h>
#include <JNetwork/Research/Command.h>
#include <JNetwork/Research/Config.h>

#include "menu.h"

USING_NS_JNET;
USING_NS_STD;
USING_NS_JC;

void PrintMenu();
bool SelectMenu(int menu);

TcpClientNetGroup* pClientGroup;


int main() {
	DefaultParserType = PacketParser::Command;

	Winsock::Initialize(2, 2);
	InitializeJCore();
	InitializeNetLogger();
	pClientGroup = dbg_new TcpClientNetGroup;
	pClientGroup->Initialize();

	for (;;) {
		PrintMenu();
		int menu;

		if (!(cin >> menu).good()) {
			cout << "메뉴를 똑바로 선택하지 않았군요, 종료합니다.\n";
			break;
		}

		if (!SelectMenu(menu)) {
			break;
		}
	}

	JCORE_DELETE_SAFE(pClientGroup);

	FinalizeNetLogger();
	FinalizeJCore();
	Winsock::Finalize();
	return 0;
}


void PrintMenu() {
	cout << "1. 로그인 서버 접속\n";
	cout << "2. 채널 서버 접속\n";
	cout << "3. 게임 서버 접속\n";
	cout << "4. 로그인 서버 메시지 전송\n";
	cout << "5. 채널 서버 메시지 전송\n";
	cout << "6. 게임 서버 메시지 전송\n";
	cout << "7. 로그인 서버 연결 종료\n";
	cout << "8. 채널 서버 연결 종료\n";
	cout << "9. 게임 서버 연결 종료\n";
	cout << "이외의 메뉴 입력시 프로그램 종료\n";
	cout << "메뉴 선택: ";
}

bool SelectMenu(int menu) {
	switch (menu) {
		case eConnectLogin:			ConnectLogin();			break;
		case eConnectChannel:		ConnectChannel();		break;
		case eConnectGame:			ConnectGame();			break;
		case eSendLogin:			SendLogin();			break;
		case eSendChannel:			SendChannel();			break;
		case eSendGame:				SendGame();				break;
		case eDisconnectLogin:		DisconnectLogin();		break;
		case eDisconnectChannel:	DisconnectChannel();	break;	
		case eDisconnectGame:		DisconnectGame();		break;
		default: return false;
	}

	return true;
}