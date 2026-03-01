/*
 * 작성자 : 윤정도
 */

#pragma once

#include <WinSock2.h>

#include <jc/Primitives/RefCountObject.h>

#include <jnet/IOCP/IOCP.h>
#include <jnet/IOCP/IOCPPostOrder.h>

// 오브젝트 풀링을 사용할 경우 4%더 성능 저하가 있음.
// 아무래도 전역 오브렉트 풀이라.. 락에서 병목 생기는 듯
// jnet_gbench프로젝트 -> BM_PacketPooling 테스트 수행 결과임.
// #define USE_OVERLAPPED_STATIC_POOL

#ifdef USE_OVERLAPPED_STATIC_POOL
#include <jc/Pool/ObjectPool.h>
#endif


NS_JNET_BEGIN

class IOCPOverlapped : public OVERLAPPED, public jc::RefCountObject
{
public:
	enum class Type
	{
		Custom,
		Accept,
		Connect,
		Disconnect,
		Receive,
		Send,
		ReceiveFrom,
		SendTo,
		Max,
	};

public:
	IOCPOverlapped(IOCP* _pIocp, Type _type);
	~IOCPOverlapped() override;

public:
	virtual void Process(BOOL _result, _u32l _bytesTransferred, IOCPPostOrder* _pCompletionKey) = 0;
	Type GetType() const { return type_; }

	void ReleaseAction() override { delete this; }
	bool IsFailed(BOOL _result, OUT _u32& _errorCode);
	bool IsFailed(SOCKET _socket, BOOL _result, _u32l _bytesTransferred, OUT _u32& _errorCode);

	static void __FreeAllObjects();
protected:
	Type type_;
	IOCP* pIocp_;
};

using IOCPOverlappedPtr = jc::SharedPtr<IOCPOverlapped>;

NS_END
