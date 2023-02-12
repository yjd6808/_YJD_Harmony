

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Research/UdpClientNetGroup.h>
#include <JNetwork/Research/Command.h>
#include <JNetwork/Research/Config.h>

#include "menu.h"

USING_NS_JNET;
USING_NS_STD;
USING_NS_JC;

void PrintMenu();
bool SelectMenu(int menu);

UdpClientNetGroup* pClientGroup;

int main() {
	Winsock::Initialize(2, 2);

	pClientGroup = dbg_new UdpClientNetGroup{ "UDP B" };
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

	DeleteSafe(pClientGroup);

	Winsock::Finalize();
	return 0;
}

void PrintMenu() {
	cout << "1. 로그인 UDP 연결\n";
	cout << "2. 채널 UDP 연결\n";
	cout << "3. 게임 UDP 연결\n";
	cout << "4. 클라A UDP 연결\n";
	cout << "5. 로그인 UDP 메시지 전송\n";
	cout << "6. 채널 UDP 메시지 전송\n";
	cout << "7. 게임 UDP 메시지 전송\n";
	cout << "8. 클라A UDP 메시지 전송\n";
	cout << "9. 바인드 + 수신대기\n";
	cout << "10. 연결 종료\n";
	cout << "메뉴 선택: ";
}

bool SelectMenu(int menu) {
	switch (menu) {
	case eConnectLogin:		ConnectLogin();		break;
	case eConnectChannel:	ConnectChannel();	break;
	case eConnectGame:		ConnectGame();		break;
	case eConnectClientA:	ConnectClientA();	break;
	case eSendLogin:		SendLogin();		break;
	case eSendChannel:		SendChannel();		break;
	case eSendGame:			SendGame();			break;
	case eSendClientA:		SendClientA();		break;
	case eBindRecvFrom:		BindRecvFrom();		break;
	case eDisconnect:		Disconnect();		break;
	default: return false;
	}

	return true;
}
