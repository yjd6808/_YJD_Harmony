﻿/*
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

#include <JCore/TypeTraits.h>
#include <JCore/Primitives/RefCountObjectPtr.h>

#include <WinSock2.h>

#include <JNetwork/Namespace.h>
#include <JNetwork/Buffer/CommandBuffer.h>
#include <JNetwork/Packet/Command.h>

NS_JNET_BEGIN


enum class PacketType
{
	Raw,
	Command
};

enum class PacketDetailType
{
	Raw,
	Static,
	Dynamic,
	Single,
	CommandBuffer
};


struct IPacket : JCore::RefCountObject
{
	IPacket() = default;
	~IPacket() override = default;

	void ReleaseAction() override { delete this; }
	virtual WSABUF GetWSABuf() const = 0;
	virtual PacketType GetType() const = 0;
	virtual PacketDetailType GetDetailType() const = 0;
};


struct RawPacket : IPacket
{
	RawPacket(char* data, int len) {
		buf.buf = data;
		buf.len = len;
	}
	WSABUF GetWSABuf() const override { return buf; }
	PacketType GetType() const override { return PacketType::Raw; }
	PacketDetailType GetDetailType() const override { return PacketDetailType::Raw; }
private:
	WSABUF buf;
};

 /*=====================================================================================
							 커맨드 패킷 객체
	 1개 이상의 커맨드를 담아서 전송할 수 있도록한다.
	 템플릿 파라미터로 모두 Command를 상속받은 타입을 전달하도록 해야한다.
	 (다른 타입은 전달 못하도록 막아놓음)

	 SendAsync()로 패킷을 송신하게 되면 IOCPOverlappedSend에서 해당 패킷을 소멸시키도록 한다.

	 <---------------------- Session Buffer ----------------------------------->
	 <--- Packet<GenericCommand<A>, GenericCommand<B>> ----><---  Packet<GenericCommand<C>> --->
	 ===================================================================================================
	CommandPacket  |  GenericCommand<A>  |  GenericCommand<B> | CommandPacket | GenericCommand<C>  |
	 ===================================================================================================
	 PACKET_HEADER_SIZE      GetPacketLength()
			↓                        ↓
			4      sizeof(GenericCommand<A>) + sizeof(GenericCommand<B>)

  =====================================================================================*/



struct CommandPacket : IPacket
{
	CommandPacket() {}
	CommandPacket(CmdCnt_t iCommandCount, PktLen_t iPacketLen)
		: m_iCommandCount(iCommandCount)
		, m_iPacketLen(iPacketLen) {
	}

	~CommandPacket() override = default;

	PktLen_t 	GetPacketLength() const { return m_iPacketLen; }
	CmdCnt_t	GetCommandCount() const { return m_iCommandCount; }
	virtual char* GetCommandSource() const = 0;		// 커맨드 시작위치 반환

	ICommand* GetCommand(int index) {
		if (index >= m_iCommandCount || index < 0)
			return nullptr;

		ICommand* pCmd = nullptr;
		char* pCmdData = GetCommandSource();
		int i = 0;
		while (i < m_iCommandCount) {
			pCmd = reinterpret_cast<ICommand*>(pCmdData);

			if (i == index) {
				break;
			}

			pCmdData += pCmd->CmdLen;
			++i;
		}
		return pCmd;
	}

	void ForEach(const JCore::Action<ICommand*>& consumer) {
		int iCmdIndex = 0;
		char* pCmdData = GetCommandSource();

		while (iCmdIndex < m_iCommandCount) {
			ICommand* pCurCmd = reinterpret_cast<ICommand*>(pCmdData);
			consumer(pCurCmd);

			if (pCurCmd->CmdLen >= 2500) {
				DebugAssert(false);
				break;
			}

			pCmdData += pCurCmd->CmdLen;
			iCmdIndex += 1;
		}
	}

	PacketType GetType() const override { return PacketType::Command; }
protected:
	CmdCnt_t m_iCommandCount{};
	PktLen_t m_iPacketLen{};		// IPacket 크기를 제외한 커맨드들의 총 크기
	// ICommand의 CmdLen은 헤더 포함이지만 이녀석은 포함안됨
};

// 패킷을 받을 때는 가상 함수 테이블이 없는 구조체로 받자.
struct RecvedCommandPacket
{
	RecvedCommandPacket() = delete;
	~RecvedCommandPacket() = delete;

	Int16U	GetPacketLength() const { return m_iPacketLen; }
	Int16U	GetCommandCount() const { return m_iCommandCount; }

	void ForEach(const JCore::Action<ICommand*>& consumer);

	// 삭제시 필히 char*로 캐스팅 후 delete[] 해줄 것
	RecvedCommandPacket* Clone() const;
protected:
	Int16U m_iCommandCount;
	Int16U m_iPacketLen;
};

/*=====================================================================================

								 스태릭 패킷 : 고정크기의 커맨드만 담는 녀석
								 
  ===================================================================================== */
template <typename... CommandArgs>
class StaticPacket : public CommandPacket
{
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command"); // 커맨드는 최소 1개 이상 전달하도록 하자.
	static_assert(JCore::IsMultipleDerived_v<ICommand, CommandArgs...>, "... CommandArgs must be derived type of \"ICommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.

	template <int Index>
	using TypeAt = JCore::IndexOf_t<Index, CommandArgs...>;	 // 인자로 전달받은 커맨드 타입
public:
	StaticPacket() : CommandPacket(CommandCount, PacketLen) {

		// m_pBuf에 각 커맨드의 시작주소 마다 디폴트 초기화를 수행해준다.
		// 예를들어서 Packet<Commant<A>, GenericCommand<B>> 패킷을 생성했다면
		// 
		// m_pBuf + 0		  에다가 A를 디폴트 초기화하고
		// m_pBuf + sizeof(A) 에다가 B를 디폴트 초기화하도록 한다.
		PlacementDefaultAllocateRecursive<CommandCount - 1, CommandArgs...>();
	}

	~StaticPacket() override = default;



	WSABUF GetWSABuf() const override {
		/*

		<---------------    StaticPacket<GenericCommand<A>, GenericCommand<B>> ----------------------->
		<------------------ IPacket ---------------- >
		<-- RefCount--> <-------- RecvedCommandPacket ----------->
		===========================================================================
		 vfptr | m_Ref | m_iCommandCount | m_iPacketLen | GenericCommand<A> | GenericCommand<B> |
		|      |       |                 m_pBuf                                   |
		===========================================================================
		↑              ↑ <----------------- 전송해줘야하는 구간 ---------------------->
	   this     this + sizeof(RefCount)

		*/
		// 패킷 상위 4바이트는 패킷 헤더로 사용한다.
		* (CmdCnt_t*)(m_pBuf + 0) = m_iCommandCount;
		*(PktLen_t*)(m_pBuf + sizeof(CmdCnt_t)) = m_iPacketLen;

		WSABUF wsaBuf;
		wsaBuf.len = PacketHeaderSize_v + m_iPacketLen;
		wsaBuf.buf = (char*)m_pBuf;
		return wsaBuf;
	}

	char* GetCommandSource() const override { return const_cast<char*>(m_pBuf) + PacketHeaderSize_v; }
	PacketDetailType GetDetailType() const override { return PacketDetailType::Static; }
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
		static_assert(Index < sizeof...(CommandArgs), "... Index must be less than parameter pack count");
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

  현재 사용금지, count가 0인 경우에 대해서 Placement New 수행시 메모리 커럽션 발생 문제가 생길 수 있음
  SinglePacket에 대해서는 해당 문제를 수정했으므로 그걸 사용하도록 할 것
  시간이 촉박하고 따이나믹 패킷을 복수로 보내는일은 아직까진 없었으므로.. 수정하진 않는다.
  추후 사용하게 된다면 필히 수정 후 사용할 것!!!
  ===================================================================================== */
template <typename... CommandArgs>
class DynamicPacket : public CommandPacket
{
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command"); // 커맨드는 최소 1개 이상 전달하도록 하자.
	static_assert(JCore::IsMultipleDerived_v<DynamicCommand, CommandArgs...>, "... CommandArgs must be derived type of \"DynamicCommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
	template <int Index>
	using TypeAt = JCore::IndexOf_t<Index, CommandArgs...>;	 // 인자로 전달받은 커맨드 타입
public:
	template <typename... Ints>
	DynamicPacket(Ints... counts) : CommandPacket() {
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

	char* GetCommandSource() const override { return const_cast<char*>(m_pDynamicBuf) + PacketHeaderSize_v; }

	template <int Index>
	TypeAt<Index>* Get() {
		static_assert(Index < CommandCount, "... Index must less then command count");
		return reinterpret_cast<TypeAt<Index>*>(CommandBuf() + m_CmdEndPos[Index]);
	}

	PacketDetailType GetDetailType() const override { return PacketDetailType::Dynamic; }
private:

	/*
	 *  m_CmdEndPos[0] = 0                                      (첫번째 커맨드 시작 위치)
	 *  m_CmdEndPos[1] = 첫번째 커맨드 길이						(두번째 커맨드 시작 위치)
	 *	m_CmdEndPos[2] = 첫번쨰 커맨드 길이 + 두번째 커맨들 길이		(세번째 커맨드 시작 위치)
	 */
	template <int Index>
	void InitializeCountRecursive(int count) {
		int iSize = TypeAt<Index>::_Size(count);

		if constexpr (Index > 0) {
			iSize += m_CmdEndPos[Index];
		}
		m_CmdEndPos[Index + 1] = iSize;
	}

	template <int Index, typename... Ints>
	void InitializeCountRecursive(int count, Ints... counts) {

		int iSize = TypeAt<Index>::_Size(count);

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


class CommandBufferPacket : public CommandPacket
{
public:
	CommandBufferPacket(const CommandBufferPtr& buffer);
	WSABUF GetWSABuf() const override {
		return { (ULONG)m_Buffer->GetWritePos(), m_Buffer->Source() };
	}
	char* GetCommandSource() const override { return m_Buffer->Source() + PacketHeaderSize_v; }
	PacketDetailType GetDetailType() const override { return PacketDetailType::CommandBuffer; }
private:
	CommandBufferPtr m_Buffer;
};




/*=====================================================================================

							   싱글 패킷 (커맨드 한개만 전송하는 용도)
							   다이나믹, 스태릭 커맨드 아무거나 가능

 TODO: 다이나믹 버퍼는 메모리풀에서 할당받도록 하는게 좋아보인다.
 ===================================================================================== */
template <typename TCommand>
class SinglePacket : public CommandPacket
{
	static_assert(JCore::IsBaseOf_v<ICommand, TCommand>, "... TCommand must be derived type of \"ICommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
public:
	SinglePacket() : SinglePacket(1, true) {}
	SinglePacket(int count, bool noCount = false)
		: CommandPacket(1, TCommand::_Size(count)) // 실제 보낼 패킷데이터 크기(m_iPacketLen)만큼만 보내기 위해서 m_iPacketLen은 올바르게 설정해줘야한다.
		, m_pDynamicBuf(dbg_new char[PacketHeaderSize_v + TCommand::_Size(count <= 0 ? 1 : count)])	// count가 0일 경우 구조체 일부가 잘리기 때문에 PlacementNew 수행시 메모리 커럽션이 발생하게 된다. 따라서 생성시에는 count가 0이더라도 1로 가정하고 처리하도록 한다.
		, Cmd(*reinterpret_cast<TCommand*>(m_pDynamicBuf + PacketHeaderSize_v))
	{
		// 다이나믹 커맨드인데 count 인자를 명시적으로 전달안한 경우
		if (JCore::IsBaseOf_v<DynamicCommand, TCommand> && noCount) {
			DebugAssertMsg(false, "다이나맥 커맨드는 명시적으로 count 인자를 전달해줘야합니다.");
		}

		// m_pDynamicBuf가 최소 sizeof(TCommand)보다는 커야지 메모리 커럽션이 발생하지 않는다.
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

	char* GetCommandSource() const override { return const_cast<char*>(m_pDynamicBuf) + PacketHeaderSize_v; }
	PacketDetailType GetDetailType() const override { return PacketDetailType::Single; }

	// @참고: https://stackoverflow.com/questions/2669888/initialization-order-of-class-data-members
	// 클래스 필드는 배열한 순서대로 초기화가 이뤄진다.
	// 따라서 참조커맨드는 무조건 다이나믹 버퍼가 초기화된 후 초기화해줘야한다.
	char* m_pDynamicBuf;
	TCommand& Cmd;
};

using ISendPacketPtr = JCore::SharedPtr<IPacket>;
using ISendPacketGuard = JCore::RefCountObjectPtr<IPacket>;

#define JNET_SEND_PACKET_AUTO_RELEASE_GUARD(packet) ISendPacketGuard JCORE_CONCAT_COUNTER(__autorelease_guard__)(packet, false)

NS_JNET_END