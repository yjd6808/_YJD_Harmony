/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:23:36 PM
 * =====================
 * 송신용 패킷 (4타입)
 *   - 멀티 스타릭 패킷 (스타릭 커맨드 여러개 담아서 전송)
 *	 - 멀티 다이나믹 패킷 (다이나믹 커맨드 여러개 담아서 전송)
 *	 - 커맨드 버퍼 패킷 (커맨드 버퍼 통째로 전송)
 *	 - 싱글 패킷 (스타릭 커맨드, 다이나믹 커맨드 암거나 1개만 담음)
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

								 스태릭 패킷 : 고정크기의 커맨드만 담는 녀석
								 
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
		 vfptr | m_Ref | m_iCommandCount | m_iPacketLen | Command<A> | Command<B> |
		|      |       |                 m_pBuf                                   |
		===========================================================================
		↑              ↑ <----------------- 전송해줘야하는 구간 ---------------------->
	   this     this + sizeof(RefCount)

		*/
		// 패킷 상위 4바이트는 패킷 헤더로 사용한다.
		*(CmdCnt_t*)(m_pBuf + 0) = m_iCommandCount;
		*(PktLen_t*)(m_pBuf + sizeof(CmdCnt_t)) = m_iPacketLen;

		WSABUF wsaBuf;
		wsaBuf.len = PacketHeaderSize_v + m_iPacketLen;
		wsaBuf.buf = (char*)m_pBuf;
		return wsaBuf;
	}

	
private:
	template <int Index, typename Cmd, typename... CmdArgs>
	constexpr void PlacementDefaultAllocateRecursive() {
		if constexpr (Index == 0) {
			:: new (Get<Index>()) TypeAt<Index>();
		} else {
			:: new (Get<Index>()) TypeAt<Index>();
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

	char* CommandBuf() const {
		return (char*)m_pBuf + PacketHeaderSize_v; // 상위 4바이트는 헤더로 사용하기 때문에
	}
public:
	
	template <int Index>
	constexpr TypeAt<Index>* Get() {
		static_assert(Index < sizeof...(CommandArgs),  "... Index must be less than parameter pack count");
		if constexpr (Index == 0) {
			return (TypeAt<Index>*)CommandBuf();
		} else {
			return (TypeAt<Index>*)(CommandBuf() + SumOfSizeRecursive<Index - 1>());
		}
	}
private:
	static constexpr int PacketLen = (... + sizeof(CommandArgs));
	static constexpr int CommandCount = sizeof...(CommandArgs);

	char m_pBuf[PacketLen + PacketHeaderSize_v];
};


/*=====================================================================================

								 따이나믹 패킷 : 가변크기의 커맨드들을 담는 녀석

  ===================================================================================== */
template <typename... CommandArgs>
class DynamicPacket : public ISendPacket
{
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command"); // 커맨드는 최소 1개 이상 전달하도록 하자.
	static_assert(JCore::IsMultipleDerived_v<DynamicCommand, CommandArgs...>, "... CommandArgs must be derived type of \"DynamicCommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
	template <int Index>
	using TypeAt = JCore::IndexOf_t<Index, CommandArgs...>;	 // 인자로 전달받은 커맨드 타입
public:
	template <typename... Ints>
	DynamicPacket(Ints... counts) : ISendPacket() {
		static_assert(CommandCount == sizeof...(counts), "... Invalid command size count");
		const int iPacketLen = (... + counts);
		InitializeCountRecursive<0>(counts...);

		this->m_iPacketLen = m_CmdEndPos[CommandCount];
		this->m_iCommandCount = CommandCount;
		this->m_pDynamicBuf = dbg_new char[PacketHeaderSize_v + this->m_iPacketLen];

		ConstructRecursive<0>(counts...);
	}
	~DynamicPacket() override {
		JCORE_DELETE_ARRAY_SAFE(m_pDynamicBuf);
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



	template <int Index>
	TypeAt<Index>* Get() {
		static_assert(Index < CommandCount, "... Index must less then command count");
		return reinterpret_cast<TypeAt<Index>*>(CommandBuf() + m_CmdEndPos[Index]);
	}
private:

	/*
	 *  m_CmdEndPos[0] = 0                                      (첫번째 커맨드 시작 위치)
	 *  m_CmdEndPos[1] = 첫번째 커맨드 길이						(두번째 커맨드 시작 위치)
	 *	m_CmdEndPos[2] = 첫번쨰 커맨드 길이 + 두번째 커맨들 길이		(세번째 커맨드 시작 위치)
	 */
	template <int Index>
	void InitializeCountRecursive(int count) {
		int iSize = TypeAt<Index>::Size(count);

		if constexpr (Index > 0) {
			iSize += m_CmdEndPos[Index];
		}
		m_CmdEndPos[Index + 1] = iSize;
	}

	template <int Index, typename... Ints>
	void InitializeCountRecursive(int count, Ints... counts) {

		int iSize = TypeAt<Index>::Size(count);

		if constexpr (Index > 0) {
			iSize += m_CmdEndPos[Index];
		}
		m_CmdEndPos[Index + 1] = iSize;
		InitializeCountRecursive<Index + 1>(counts...);
	}

	template <int Index>
	void ConstructRecursive(int count) {
		:: new (Get<Index>()) TypeAt<Index>(count);
	}

	template <int Index, typename... Ints>
	void ConstructRecursive(int count, Ints... counts) {
		:: new (Get<Index>()) TypeAt<Index>(count);
		ConstructRecursive<Index + 1>(counts...);
	}

	char* CommandBuf() const {
		return m_pDynamicBuf + PacketHeaderSize_v; // m_pDynamicBuf의 상위 4바이트는 헤더로 사용하기 때문에
	}
private:
	static constexpr int CommandCount = sizeof...(CommandArgs);

	int m_CmdEndPos[CommandCount + 1]{};		// 각 커맨드 길이 임시 기록용
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




/*=====================================================================================

							   싱글 패킷 (커맨드 한개만 전송하는 용도)
							   다이나믹, 스태릭 커맨드 아무거나 가능
  ===================================================================================== */
template <typename TCommand>
class SinglePacket : public ISendPacket
{
	static_assert(JCore::IsBaseOf_v<ICommand, TCommand>, "... TCommand must be derived type of \"ICommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
public:
	SinglePacket() : SinglePacket(1, true) {}
	SinglePacket(int count, bool noCount = false)
		: ISendPacket(1, TCommand::Size(count))
		, m_pDynamicBuf(dbg_new char[PacketHeaderSize_v + this->m_iPacketLen])
		, Cmd(*reinterpret_cast<TCommand*>(m_pDynamicBuf + PacketHeaderSize_v))
	{
		// 다이나믹 커맨드인데 count 인자를 명시적으로 전달안한 경우
		if (JCore::IsBaseOf_v<DynamicCommand, TCommand> && noCount) {
			DebugAssertMsg(false, "다이나맥 커맨드는 명시적으로 count 인자를 전달해줘야합니다.");
		}

		JCore::Memory::PlacementNew(Cmd, count);
	}
	~SinglePacket() override {
		JCORE_DELETE_ARRAY_SAFE(m_pDynamicBuf);
	}

	WSABUF GetWSABuf() const override {
		*(CmdCnt_t*)(m_pDynamicBuf + 0) = m_iCommandCount;
		*(PktLen_t*)(m_pDynamicBuf + sizeof(CmdCnt_t)) = m_iPacketLen;

		WSABUF wsaBuf;
		wsaBuf.len = PacketHeaderSize_v + m_iPacketLen;
		wsaBuf.buf = m_pDynamicBuf;
		return wsaBuf;
	}

	// @참고: https://stackoverflow.com/questions/2669888/initialization-order-of-class-data-members
	// 클래스 필드는 배열한 순서대로 초기화가 이뤄진다.
	// 따라서 참조커맨드는 무조건 다이나믹 버퍼가 초기화된 후 초기화해줘야한다.
	char* m_pDynamicBuf;
	TCommand& Cmd;
};



using ISendPacketPtr = JCore::SharedPtr<ISendPacket>;

NS_JNET_END