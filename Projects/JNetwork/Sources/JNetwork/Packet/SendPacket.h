/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:23:36 PM
 * =====================
 * 송신용 패킷
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/TypeCast.h>
#include <JCore/TypeTraits.h>
#include <JCore/SafeRefCount.h>

#include <WinSock2.h>

#include <JNetwork/Namespace.h>
#include <JNetwork/Buffer/CommandBuffer.h>
#include <JNetwork/Packet/Command.h>





NS_JNET_BEGIN

 /*=====================================================================================
								 패킷 객체
		 1개 이상의 커맨드를 담아서 전송할 수 있도록한다.
		 템플릿 파라미터로 모두 Command를 상속받은 타입을 전달하도록 해야한다.
		 (다른 타입은 전달 못하도록 막아놓음)

		 SendAsync()로 패킷을 송신하게 되면 IOCPOverlappedSend에서 해당 패킷을 소멸시키도록 한다.



		 <---------------------- Session Buffer ----------------------------------->
		 <--- Packet<Command<A>, Command<B>> ----><---  Packet<Command<C>> --->
		 ===========================================================================
		ISendPacket  |  Command<A>  |  Command<B> | ISendPacket | Command<C>  |
		 ===========================================================================
		 PACKET_HEADER_SIZE      GetPacketLength()
				↓                        ↓
				4      sizeof(Command<A>) + sizeof(Command<B>)

  =====================================================================================*/





struct ISendPacket : JCore::SafeRefCount
{
	ISendPacket() {}
	ISendPacket(CmdCnt_t iCommandCount, PktLen_t iPacketLen)
		: m_iCommandCount(iCommandCount)
		, m_iPacketLen(iPacketLen) {
	}

	~ISendPacket() override = default;

	PktLen_t	GetPacketLength() const { return m_iPacketLen; }
	CmdCnt_t	GetCommandCount() const { return m_iCommandCount; }
	void ReleaseAction() override { delete this; }
	virtual WSABUF GetWSABuf() const = 0;
protected:
	CmdCnt_t m_iCommandCount{};
	PktLen_t m_iPacketLen{};		// IPacket 크기를 제외한 커맨드들의 총 크기
								// ICommand의 CmdLen은 헤더 포함이지만 이녀석은 포함안됨
};




/*=====================================================================================

								 스태릭 패킷 : 고정크기의 커맨드만 사용
								 
  ===================================================================================== */
template <typename... CommandArgs>
class StaticPacket : public ISendPacket
{
	static_assert(sizeof...(CommandArgs) > 0,  "... Packet must have one more command"); // 커맨드는 최소 1개 이상 전달하도록 하자.
	static_assert(JCore::IsMultipleDerived_v<ICommand, CommandArgs...>,  "... CommandArgs must be derived type of \"ICommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.

	template <int Index>
	using TypeAt = JCore::IndexOf_t<Index, CommandArgs...>;	 // 인자로 전달받은 커맨드 타입
public:
	StaticPacket() : ISendPacket(CommandCount, PacketLen) {

		// m_pBuf에 각 커맨드의 시작주소 마다 디폴트 초기화를 수행해준다.
		// 예를들어서 Packet<Commant<A>, Command<B>> 패킷을 생성했다면
		// 
		// m_pBuf + 0		  에다가 A를 디폴트 초기화하고
		// m_pBuf + sizeof(A) 에다가 B를 디폴트 초기화하도록 한다.
		PlacementDefaultAllocateRecursive<CommandCount - 1, CommandArgs...>();
	}

	~StaticPacket() override = default;

	

	WSABUF GetWSABuf() const override {
		/*

		<---------------    StaticPacket<Command<A>, Command<B>> ----------------------->
		<------------------ ISendPacket ---------------- >
		<-- RefCount--> <-------- IRecvPacket ----------->
		===========================================================================
		 vfptr | m_Ref | m_iCommandCount | m_iPacketLen | Command<A> | Command<B>
		===========================================================================
		↑              ↑ <----------------- 전송해줘야하는 구간 ---------------------->
	   this     this + sizeof(RefCount)

		*/
		WSABUF wsaBuf;
		wsaBuf.len = PacketHeaderSize_v + m_iPacketLen;
		wsaBuf.buf = (char*)this + sizeof(SafeRefCount);
		return wsaBuf;
	}

	
private:
	template <int Index, typename Cmd, typename... CmdArgs>
	constexpr void PlacementDefaultAllocateRecursive() {
		if constexpr (Index == 0) {
			::new (Get<Index>()) Cmd();
		} else {
			::new (Get<Index>()) Cmd();
			PlacementDefaultAllocateRecursive<Index - 1, CmdArgs...>();
		}
	}

	template <int Index>
	constexpr int SumOfSizeRecursive() {
		if constexpr (Index == 0) {
			return sizeof(TypeAt<0>);
		} else {
			return sizeof(TypeAt<Index>) + SumOfSizeRecursive<Index - 1>();
		}
	}

public:
	
	template <int Index>
	constexpr TypeAt<Index>* Get() {
		static_assert(Index < sizeof...(CommandArgs),  "... Index must be less than parameter pack count");
		if constexpr (Index == 0) {
			return (TypeAt<Index>*)m_pBuf;
		} else {
			return (TypeAt<Index>*)(m_pBuf + SumOfSizeRecursive<Index - 1>());
		}
	}
private:
	static constexpr int PacketLen = (... + sizeof(CommandArgs));
	static constexpr int CommandCount = sizeof...(CommandArgs);

	char m_pBuf[PacketLen];
};


/*=====================================================================================

								 따이나믹 패킷 : 가변크기의 패킷 사용

  ===================================================================================== */
template <typename... CommandArgs>
class DynamicPacket : public ISendPacket
{
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command"); // 커맨드는 최소 1개 이상 전달하도록 하자.
	static_assert(JCore::IsMultipleDerived_v<ICommand, CommandArgs...>, "... CommandArgs must be derived type of \"ICommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
	template <int Index>
	using TypeAt = JCore::IndexOf_t<Index, CommandArgs...>;	 // 인자로 전달받은 커맨드 타입
public:
	template <typename... Ints>
	DynamicPacket(Ints... sizes) : ISendPacket() {
		static_assert(CommandCount == sizeof...(sizes), "... Invalid command size count");
		const int iPacketLen = (... + sizes);
		this->m_iPacketLen = iPacketLen;
		this->m_iCommandCount = CommandCount;
		this->m_pDynamicBuf = dbg_new char[PacketHeaderSize_v + iPacketLen];

		InitializeCommandLenRecursive<0>(sizes...);
	}
	~DynamicPacket() override {
		DeleteArraySafe(m_pDynamicBuf);
	}


	WSABUF GetWSABuf() const override {
		// 패킷 상위 4바이트는 패킷 헤더로 사용한다.
		*(CmdCnt_t*)(m_pDynamicBuf + 0) = m_iCommandCount;
		*(PktLen_t*)(m_pDynamicBuf + sizeof(CmdCnt_t)) = m_iPacketLen;

		WSABUF wsaBuf;
		wsaBuf.len = PacketHeaderSize_v + m_iPacketLen;
		wsaBuf.buf = m_pDynamicBuf;
		return wsaBuf;
	}

	template <int Index, typename... Args>
	void Construct(Args&&... args) {
		static_assert(Index < CommandCount, "... Index must less then command count");
		auto pCmd = ::new (CommandBuf() + CommandPos(Index)) TypeAt<Index>{ JCore::Forward<Args>(args)... };
		pCmd->SetCommandLen(m_pCommandSizes[Index]);
	}

	template <int Index>
	TypeAt<Index>* Get() {
		static_assert(Index < CommandCount, "... Index must less then command count");
		return reinterpret_cast<TypeAt<Index>*>(CommandBuf() + CommandPos(Index));
	}
private:
	template <int Index>
	void InitializeCommandLenRecursive(int size) {
		m_pCommandSizes[Index] = size;
		reinterpret_cast<TypeAt<Index>*>(CommandBuf() + CommandPos(Index))->SetCommandLen(size);
	}

	template <int Index, typename... Ints>
	void InitializeCommandLenRecursive(int size, Ints... sizes) {
		m_pCommandSizes[Index] = size;
		reinterpret_cast<TypeAt<Index>*>(CommandBuf() + CommandPos(Index))->SetCommandLen(size);
		InitializeCommandLenRecursive<Index + 1>(sizes...);
	}

	int CommandPos(const int idx) const {
		if (idx == 0) {
			return 0;
		}

		char* pCurBuf = CommandBuf();
		int iCommandPos = 0;

		for (int i = 1; i <= idx; ++i) {
			const int iCurCommandLen = reinterpret_cast<ICommand*>(pCurBuf)->GetCommandLen();
			iCommandPos += iCurCommandLen;
			pCurBuf = pCurBuf + iCurCommandLen;
		}

		return iCommandPos;
	}



	char* CommandBuf() const {
		return m_pDynamicBuf + PacketHeaderSize_v; // m_pDynamicBuf의 상위 4바이트는 헤더로 사용하기 때문에
	}

	
private:
	static constexpr int CommandCount = sizeof...(CommandArgs);

	int m_pCommandSizes[CommandCount]{};		// 각 커맨드 길이 임시 기록용
	char* m_pDynamicBuf;
};

/*=====================================================================================

		커맨드버퍼 패킷 : 커맨드만 담긴 버퍼를 패킷으로 감싸는 용도 및 커맨드 버퍼 산소호흡기 느낌
		 - 버퍼에 레퍼런스 카운트를 추가하면 구조가 깨짐
		 - 기존 ISendPacket을 활용하고 싶음
		 - 산소호흡기(ref count)를 부착해서 안정적으로 송신하기 위함

  ===================================================================================== */


class CommandBufferPacket : public ISendPacket
{
public:
	CommandBufferPacket(const CommandBufferPtr& buffer);

	WSABUF GetWSABuf() const override {
		return { (ULONG)m_Buffer->GetWritePos(), m_Buffer->Source() };
	}

private:
	CommandBufferPtr m_Buffer;
};


using ISendPacketPtr = JCore::SharedPtr<ISendPacket>;

NS_JNET_END