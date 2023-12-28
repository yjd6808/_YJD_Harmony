/*
 * �ۼ���: ������
 * ������: 12/25/2023 9:39:58 PM (ũ��������)
 * =====================
 *
 */

#include <JNetwork/Network.h>
#include <JNetwork/Packet/PacketParser.h>

NS_JNET_BEGIN

 
/* �ɼ��� �ӽ� �۽� ���۸��� ���� �ʵ��� �Ѵ�.
 *
 * ���� �۽� ���� ũ�⸦ 0���� ���������ν� �츮�� ������ WSABUF�� ���ۿ��� ������ �ϵ��� �Ѵ�.
 * ���� Ƚ���� 1�� ���� �� �־ �۽� ������ ��� ��ų �� �ִ�.
 *  -> ���� �׽�Ʈ �غ��� ������ Ȯ���� ��������.
 *
 * TODO:�ϴ��� ���̽��� ���ؼ� �����غ� ��
 * 1. Nagle + DisableSendBuffering�� Send���۰� ���⶧���� �۽��� �����ȴٴµ� Ȯ���غ� ��
 * ���ó���: https://chfhrqnfrhc.tistory.com/entry/SOSNDBUF-0?category=484325
 *
 */
bool DisableSendBuffering = true; 


/* �ɼ��� �ӽ� ���� ���۸��� ���� �ʵ��� �Ѵ�.
 *
 * �ٵ� Overlapped ���� IO�� ���Ź��۸��� �ϵ��� ���ϵ��� ������� WSARecv�� ������ ���ۿ� ����ȴٰ� �ϴµ�? (���ó���: https://crmerry.tistory.com/29)
 *  -> �´¸� ������ ������ �׽�Ʈ�غ��ϱ� ���� ���۸��� �ϵ��� ���ϵ��� ���� ���̰� ������.
 *
 * �׷��ٸ� ���Ź��� ũ�⸦ 0���� ����°� �ƹ� ȿ���� ���ٴ� ���ΰ�?
 * https://github.com/microsoft/Windows-classic-samples/blob/44d192fd7ec6f2422b7d023891c5f805ada2c811/Samples/Win7Samples/netds/winsock/iocp/serverex/IocpServerex.Cpp#L355
 * �� �ڵ带 ���� ���Ź��� ũ�⸦ 0���� ����� �� ��Ʈ��ũ �����ս��� ������ �� �ִٰ� ���� ���۸��� ��Ȱ��ȭ���� �ʵ��� �����ϰ� �ִ�.
 * ���� �츮�� WSARecv�� �ɾ���� �ʰų� ����ũ�Ⱑ ���ġ ������ WindowSize�� 0���� �����ع����� ������ ������ �Ұ��������� �ȴٰ� �����ִ�.
 *
 * https://github.com/torvalds/linux/blob/861deac3b092f37b2c5e6871732f3e11486f7082/net/core/sock.c#L1250
 * https://github.com/torvalds/linux/blob/861deac3b092f37b2c5e6871732f3e11486f7082/net/core/sock.c#L960
 * �������� ��� �ý��� SO_RCVBUF�� ũ�Ⱑ �ּ� SOCK_MIN_RCVBUF(2048 + sizeof(struct sk_buff))ũ�� �̻����� �����ǵ��� �ϰ� �ִ�.
 * 0���� �����Ѵٰ��ؼ� ���Ź��� ũ�⸦ 0���� �������� �ʴ� ������ ���δ�.
 *
 * ���� �����쵵 ���������� ��Ʈ��ũ �������� �����ս��� ���ؼ� ���� ���� ũ�⸦ ������ 0���� �����ϴ°� ���� ���� ������ �����غ���.
 *
 */ 
bool DisableRecvBuffering = false;


// ������ ����Ʈ ���� ũ��
int DefaultSendBufferSize = 6000;
int DefaultRecvBufferSize = 6000;


/*
 * ������ �����͸� ��� �Ľ�����, �ļ� ������ ���� ���� ��� ����Ʈ�� ������ ��Ŷ�ļ�
 * Raw: �ܼ� char �迭�� �Ľ���
 * Command: ICommand������ �Ľ���
 */
int DefaultParserType = PacketParser::Type::Raw;


/*
 * ȣ��Ʈ ������ ����ŷ �������� �������� ���� ����
 */
bool NonblokingSocket = true;

NS_JNET_END