/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 1:41:58 PM
 * =====================
 *
 */

#include <jnet/Core.h>
#include <jnet/Winsock.h>
#include <jnet/Config.h>
#include <jnet/Host/Host.h>


NS_JNET_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
bool Host::CreateSocket(TransportProtocol _protocol, bool _nonBlocking)
{
	if (socket_.IsValid())
	{
		socket_.Close();
	}

	socket_ = Socket::CreateV4(_protocol, true);

	if (!socket_.IsValid())
	{
		return false;
	}

	if (socket_.Option().SetNonBlockingEnabled(_nonBlocking) == SOCKET_ERROR)
	{
		jc_assert_msg(false, "논블로킹 소켓 설정 실패 (%u)", Winsock::LastError());
	}

	/*
	 * 처음 소켓을 생성 후 송/수신 버퍼 크기 변경전 Send/Recv 버퍼 크기 모두 65535로 나옴.
	 * 버퍼크기를 수정해주면 내가 변경해준 값으로 올바르게 나온걸 확인했음
	 *
	 * 버퍼링 여부는 jnet/Config.h 파일 참고
	 */

	if (DisableSendBuffering && socket_.Option().SetSendBufferSize(0) == SOCKET_ERROR)
	{
		jc_assert_msg(false, "소켓 %s 버퍼링 비활성화 실패 (%u)", TransmissionName(Transmission::Send), Winsock::LastError());
	}

	if (DisableRecvBuffering && socket_.Option().SetRecvBufferSize(0) == SOCKET_ERROR)
	{
		jc_assert_msg(false, "소켓 %s 버퍼링 비활성화 실패 (%u)", TransmissionName(Transmission::Recv), Winsock::LastError());
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Host::ConnectIocp()
{
	if (!pIocp_->Connect(reinterpret_cast<_whandle>(SocketHandle()), NULL))
	{
		return false;
	}

	return iocpConnected_ = true;
}


NS_END
