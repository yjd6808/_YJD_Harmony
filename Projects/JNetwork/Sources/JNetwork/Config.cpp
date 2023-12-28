/*
 * 작성자: 윤정도
 * 생성일: 12/25/2023 9:39:58 PM (크리스마스)
 * =====================
 *
 */

#include <JNetwork/Network.h>
#include <JNetwork/Packet/PacketParser.h>

NS_JNET_BEGIN

 
/* 옵션을 켤시 송신 버퍼링을 하지 않도록 한다.
 *
 * 소켓 송신 버퍼 크기를 0으로 설정함으로써 우리가 제공한 WSABUF의 버퍼에서 전송을 하도록 한다.
 * 복사 횟수를 1번 줄일 수 있어서 송신 성능을 향상 시킬 수 있다.
 *  -> 실제 테스트 해보니 성능이 확실히 좋아졌다.
 *
 * TODO:하단의 케이스에 대해서 연구해볼 것
 * 1. Nagle + DisableSendBuffering시 Send버퍼가 없기때문에 송신이 지연된다는데 확인해볼 것
 * 관련내용: https://chfhrqnfrhc.tistory.com/entry/SOSNDBUF-0?category=484325
 *
 */
bool DisableSendBuffering = true; 


/* 옵션을 켤시 수신 버퍼링을 하지 않도록 한다.
 *
 * 근데 Overlapped 수신 IO는 수신버퍼링을 하든지 안하든지 상관없이 WSARecv에 제공된 버퍼에 복사된다고 하는데? (관련내용: https://crmerry.tistory.com/29)
 *  -> 맞는말 같은게 실제로 테스트해보니까 수신 버퍼링을 하든지 안하든지 성능 차이가 없었다.
 *
 * 그렇다면 수신버퍼 크기를 0으로 만드는게 아무 효과가 없다는 말인가?
 * https://github.com/microsoft/Windows-classic-samples/blob/44d192fd7ec6f2422b7d023891c5f805ada2c811/Samples/Win7Samples/netds/winsock/iocp/serverex/IocpServerex.Cpp#L355
 * 위 코드를 보면 수신버퍼 크기를 0으로 만드는 건 네트워크 퍼포먼스를 저해할 수 있다고 수신 버퍼링을 비활성화하지 않도록 권장하고 있다.
 * 만약 우리가 WSARecv를 걸어놓지 않거나 버퍼크기가 충분치 않으면 WindowSize를 0으로 설정해버려서 데이터 수신이 불가능해지게 된다고 적혀있다.
 *
 * https://github.com/torvalds/linux/blob/861deac3b092f37b2c5e6871732f3e11486f7082/net/core/sock.c#L1250
 * https://github.com/torvalds/linux/blob/861deac3b092f37b2c5e6871732f3e11486f7082/net/core/sock.c#L960
 * 리눅스의 경우 시스템 SO_RCVBUF의 크기가 최소 SOCK_MIN_RCVBUF(2048 + sizeof(struct sk_buff))크기 이상으로 설정되도록 하고 있다.
 * 0으로 설정한다고해서 수신버퍼 크기를 0으로 설정되지 않는 것으로 보인다.
 *
 * 따라서 윈도우도 마찬가지로 네트워크 안정성과 퍼포먼스를 위해서 수신 버퍼 크기를 강제로 0으로 설정하는걸 막고 있지 않을까 추측해본다.
 *
 */ 
bool DisableRecvBuffering = false;


// 세션의 디폴트 버퍼 크기
int DefaultSendBufferSize = 6000;
int DefaultRecvBufferSize = 6000;


/*
 * 수신한 데이터를 어떻게 파싱할지, 파서 설정을 하지 않을 경우 디폴트로 생성할 패킷파서
 * Raw: 단순 char 배열로 파싱함
 * Command: ICommand단위로 파싱함
 */
int DefaultParserType = PacketParser::Type::Raw;


/*
 * 호스트 생성시 논블로킹 소켓으로 생성할지 말지 여부
 */
bool NonblokingSocket = true;

NS_JNET_END