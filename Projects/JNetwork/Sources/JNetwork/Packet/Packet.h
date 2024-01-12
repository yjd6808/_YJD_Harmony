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

#include <JCore/Macro.h>
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
	SingleStatic,
	SingleDynamic,
	CommandBuffer
};

// 매크로 처리됬기 때문에 라인 디버깅을 할 수가 없기 때문에 상당히 난감해!
#define JNET_PACKET_POOLING_PARAMS_0()							const JCore::MemoryPoolAbstractPtr& allocator 
#define JNET_PACKET_POOLING_PARAMS_1(argty1)					const JCore::MemoryPoolAbstractPtr& allocator, argty1 arg1
#define JNET_PACKET_POOLING_PARAMS_2(argty1, argty2)			const JCore::MemoryPoolAbstractPtr& allocator, argty1 arg1, argty2 arg2
#define JNET_PACKET_POOLING_PARAMS_3(argty1, argty2, argty3)	const JCore::MemoryPoolAbstractPtr& allocator, argty1 arg1, argty2 arg2, argty3 arg3

#define JNET_PACKET_POOLING_ARGS_0()							
#define JNET_PACKET_POOLING_ARGS_1(argty1)						arg1
#define JNET_PACKET_POOLING_ARGS_2(argty1, argty2)				arg1, arg2
#define JNET_PACKET_POOLING_ARGS_3(argty1, argty2, argty3)		arg1, arg2, arg3

#define JNET_PACKET_POOLING_ARGS_WITH_PACKET_0()							pPacket, allocator
#define JNET_PACKET_POOLING_ARGS_WITH_PACKET_1(argty1)						pPacket, allocator, arg1
#define JNET_PACKET_POOLING_ARGS_WITH_PACKET_2(argty1, argty2)				pPacket, allocator, arg1, arg2
#define JNET_PACKET_POOLING_ARGS_WITH_PACKET_3(argty1, argty2, argty3)		pPacket, allocator, arg1, arg2, arg3

#define JNET_PACKET_POOLING_PARAMS(...)							JCORE_EXPAND_1(JCORE_CONCAT_2(JNET_PACKET_POOLING_PARAMS_, JCORE_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define JNET_PACKET_POOLING_ARGS(...)							JCORE_EXPAND_1(JCORE_CONCAT_2(JNET_PACKET_POOLING_ARGS_,   JCORE_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define JNET_PACKET_POOLING_ARGS_WITH_PACKET(...)				JCORE_EXPAND_1(JCORE_CONCAT_2(JNET_PACKET_POOLING_ARGS_WITH_PACKET_,   JCORE_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))

#define JNET_PACKET_POOLING_CREATE(...)														\
static TPacket* Create(JNET_PACKET_POOLING_PARAMS(__VA_ARGS__)) {							\
	if (allocator == nullptr) {																\
		return dbg_new TPacket(JNET_PACKET_POOLING_ARGS(__VA_ARGS__));						\
	}																						\
	int _;																					\
	TPacket* pPacket = (TPacket*)allocator->DynamicPop(sizeof(TPacket), _);					\
	JCore::Memory::PlacementNew(JNET_PACKET_POOLING_ARGS_WITH_PACKET(__VA_ARGS__));			\
	return pPacket;																			\
}


// spGuard를 둔 이유: allocator의 레퍼런스 카운트가 1남은 경우 패킷 소멸자 호출시 allocator가 소멸되어버리는 문제가 발생해버릴 수 있다.
// 따라서 레퍼런스 카운트를 1올려놓은 후 패킷 반환이 이뤄진 이후에 allocator의 ref count를 낮추도록 한다.
// allocator가 없는 경우는 dbg_new TPacket과 같이 동적할당해서 생성해버린 경우이거나 스택할당해서 생성해버린 경우이다.
// TODO: 어떻게하면 spGuard를 사용하지 않을 수 있을까?
//  -> 이 풀이 네트워킹동안 절대 소멸되지 않음을 보장해줘야한다
//  -> 즉, 개발자가 풀의 LifeCycle을 관리해줘야한다는 말이다. ㅠㅠ
//  -> 그러면 개발자가 LifeCycle을 관리하지 않을 수는 없을까? ... 이건 최후의 최적화 방법으로 남겨놓도록 하자.
#define JNET_PACKET_POOLING_RELEASE(class_name)											\
void ReleaseAction() override {															\
	if (!m_Allocator.Exist()) {															\
		delete this;																	\
		return;																			\
	}																					\
																						\
	JCore::MemoryPoolAbstractPtr spGuard = m_Allocator;									\
	JCore::MemoryPoolAbstract* pAllocator = m_Allocator.GetPtr();						\
	this->~class_name();																\
	pAllocator->DynamicPush(this, sizeof(TPacket));										\
}



struct IPacket : JCore::RefCountObject
{
	IPacket() = default;
	IPacket(const JCore::MemoryPoolAbstractPtr& allocator) : m_Allocator(allocator) {}
	~IPacket() override = default;

	virtual WSABUF GetWSABuf() const = 0;
	virtual PacketType GetType() const = 0;
	virtual PacketDetailType GetDetailType() const = 0;
	virtual int SizeOf() const = 0;
protected:
	JCore::MemoryPoolAbstractPtr m_Allocator;	// 소멸시 반환될 메모리풀
};


struct RawPacket : IPacket
{
	using TPacket = RawPacket;

	RawPacket(char* data, int len) {
		buf.buf = data;
		buf.len = len;
	}

	RawPacket(const JCore::MemoryPoolAbstractPtr& allocator, char* data, int len) : IPacket(allocator) {
		buf.buf = data;
		buf.len = len;
	}

	JNET_PACKET_POOLING_CREATE(char*, int)
	JNET_PACKET_POOLING_RELEASE(RawPacket)

	WSABUF GetWSABuf() const override { return buf; }
	PacketType GetType() const override { return PacketType::Raw; }
	PacketDetailType GetDetailType() const override { return PacketDetailType::Raw; }
	int SizeOf() const override { return sizeof(TPacket); }
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
	CommandPacket() = default;
	CommandPacket(const JCore::MemoryPoolAbstractPtr& allocator) : IPacket(allocator) {}

	CommandPacket(CmdCnt_t iCommandCount, PktLen_t iPacketLen)
		: m_iCommandCount(iCommandCount)
		, m_iPacketLen(iPacketLen) {
	}
	CommandPacket(const JCore::MemoryPoolAbstractPtr& allocator, CmdCnt_t iCommandCount, PktLen_t iPacketLen)
		: IPacket(allocator)
		, m_iCommandCount(iCommandCount)
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
	using TPacket = StaticPacket<CommandArgs...>;
public:
	StaticPacket() : CommandPacket(CommandCount, PacketLen) {
		// m_pBuf에 각 커맨드의 시작주소 마다 디폴트 초기화를 수행해준다.
		// 예를들어서 Packet<Commant<A>, GenericCommand<B>> 패킷을 생성했다면
		// 
		// m_pBuf + 0		  에다가 A를 디폴트 초기화하고
		// m_pBuf + sizeof(A) 에다가 B를 디폴트 초기화하도록 한다.
		PlacementDefaultAllocateRecursive<CommandCount - 1, CommandArgs...>();
	}

	StaticPacket(const JCore::MemoryPoolAbstractPtr& allocator) : CommandPacket(allocator, CommandCount, PacketLen) {
		PlacementDefaultAllocateRecursive<CommandCount - 1, CommandArgs...>();
	}

	~StaticPacket() override = default;

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_RELEASE(StaticPacket)

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
		*(CmdCnt_t*)(m_pBuf + 0) = m_iCommandCount;
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

	int SizeOf() const override { return sizeof(TPacket); }
private:
	static constexpr int PacketLen = (... + sizeof(CommandArgs));
	static constexpr int CommandCount = sizeof...(CommandArgs);

	char m_pBuf[PacketLen + PacketHeaderSize_v];
};


/*=====================================================================================

								 따이나믹 패킷 : 가변크기의 커맨드들을 담는 녀석

  TODO: 현재 사용금지, count가 0인 경우에 대해서 Placement New 수행시 메모리 커럽션 발생 문제가 생길 수 있음
  SinglePacket에 대해서는 해당 문제를 수정했으므로 그걸 사용하도록 할 것
  시간이 촉박하고 따이나믹 패킷을 복수로 보내는일은 아직까진 없었으므로.. 수정하진 않는다.
  추후 사용하게 된다면 필히 수정 후 사용할 것!!!
  이걸 해결할려면 다이나믹 커맨드의 CountableObject 배열의 크기를 0으로 설정해놓도록 해야한다.
  ===================================================================================== */
template <typename... CommandArgs>
class DynamicPacket : public CommandPacket
{
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command"); // 커맨드는 최소 1개 이상 전달하도록 하자.
	static_assert(JCore::IsMultipleDerived_v<DynamicCommand, CommandArgs...>, "... CommandArgs must be derived type of \"DynamicCommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
	template <int Index>
	using TypeAt = JCore::IndexOf_t<Index, CommandArgs...>;	 // 인자로 전달받은 커맨드 타입
	using TPacket = DynamicPacket<CommandArgs...>;

public:
	// Ints가 모두 정수형일 때만 동작하도록 한다.
	template <typename... Ints, typename = JCore::DefaultEnableIf_t<JCore::IsAllUnaryTrue_v<JCore::Detail::IsIntegerType, Ints...>>>
	DynamicPacket(Ints... counts) : CommandPacket() {
		static_assert(CommandCount == sizeof...(counts), "... Invalid command size count");
		const int iPacketLen = (... + counts);
		InitializeCountRecursive<0>(counts...);

		this->m_iPacketLen = m_CmdEndPos[CommandCount];
		this->m_iCommandCount = CommandCount;
		this->m_pDynamicBuf = dbg_new char[PacketHeaderSize_v + this->m_iPacketLen];

		ConstructRecursive<0>(counts...);
	}

	template <typename... Ints, typename = JCore::DefaultEnableIf_t<JCore::IsAllUnaryTrue_v<JCore::Detail::IsIntegerType, Ints...>>>
	DynamicPacket(const JCore::MemoryPoolAbstractPtr& allocator, Ints... counts) : CommandPacket(allocator) {
		static_assert(CommandCount == sizeof...(counts), "... Invalid command size count");
		const int iPacketLen = (... + counts);
		InitializeCountRecursive<0>(counts...);

		this->m_iPacketLen = m_CmdEndPos[CommandCount];
		this->m_iCommandCount = CommandCount;
		this->m_pDynamicBuf = allocator->DynamicPop(PacketHeaderSize_v + this->m_iPacketLen);

		ConstructRecursive<0>(counts...);
	}

	~DynamicPacket() override {
		if (m_Allocator.Exist()) {
			m_Allocator->DynamicPush(m_pDynamicBuf, PacketHeaderSize_v + this->m_iPacketLen);
		} else {
			JCORE_DELETE_ARRAY_SAFE(m_pDynamicBuf);
		}
	}

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_RELEASE(DynamicPacket)

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

	int SizeOf() const override { return sizeof(TPacket); }
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
		using TCommand = TypeAt<Index>;
		using TCountableElement = typename TCommand::TCountableElement;

		TCommand* pCmd = Get<Index>();
		:: new (pCmd) TCommand(count);

		if constexpr (TCommand::ConstructCountableElement) {
			for (int i = 0; i < count - 1; ++i) {
				char* pCountableElementPos = reinterpret_cast<char*>(pCmd) + sizeof(TCommand) + sizeof(TCountableElement) * i;
				JCore::Memory::PlacementNew(*reinterpret_cast<TCountableElement*>(pCountableElementPos));
			}
		}
	}

	template <int Index, typename... Ints>
	void ConstructRecursive(int count, Ints... counts) {
		using TCommand = TypeAt<Index>;
		using TCountableElement = typename TCommand::TCountableElement;

		TCommand* pCmd = Get<Index>();
		:: new (pCmd) TCommand(count);

		if constexpr (TCommand::ConstructCountableElement) {
			for (int i = 0; i < count - 1; ++i) {
				char* pCountableElementPos = reinterpret_cast<char*>(pCmd) + sizeof(TCommand) + sizeof(TCountableElement) * i;
				JCore::Memory::PlacementNew(*reinterpret_cast<TCountableElement*>(pCountableElementPos));
			}
		}

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
	using TPacket = CommandBufferPacket;
public:
	CommandBufferPacket(const CommandBufferPtr& buffer);
	CommandBufferPacket(const JCore::MemoryPoolAbstractPtr& allocator, const CommandBufferPtr& buffer);

	JNET_PACKET_POOLING_CREATE(const CommandBufferPtr&)
	JNET_PACKET_POOLING_RELEASE(CommandBufferPacket)

	WSABUF GetWSABuf() const override { return { (ULONG)m_Buffer->GetWritePos(), m_Buffer->Source() }; }
	char* GetCommandSource() const override { return m_Buffer->Source() + PacketHeaderSize_v; }
	PacketDetailType GetDetailType() const override { return PacketDetailType::CommandBuffer; }
	int SizeOf() const override { return sizeof(TPacket); }
private:
	CommandBufferPtr m_Buffer;
};




/*=====================================================================================

							   싱글 패킷 (커맨드 한개만 전송하는 용도)
							   다이나믹, 스태릭 커맨드 아무거나 가능

 TODO[완료]: 다이나믹 버퍼는 메모리풀에서 할당받도록 하는게 좋아보인다.
 ===================================================================================== */
template <typename TCommand, bool IsStatic = TCommand::IsStatic>
class SinglePacket;

// 스타릭 커맨드 전용
template <typename TCommand>
class SinglePacket<TCommand, true> : public CommandPacket {
	static_assert(JCore::IsBaseOf_v<ICommand, TCommand>, "... TCommand must be derived type of \"ICommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
	using TPacket = SinglePacket<TCommand>;
public:
	// count와 noCount 매개변수는 다이나믹 커맨드 처리를 위해 특수화된 SinglePacket과의 호환성을 위해 둔 것이다.
	SinglePacket(int count = 1)
		: CommandPacket(1, sizeof(TCommand))
		, Cmd(*reinterpret_cast<TCommand*>(m_pBuf + PacketHeaderSize_v))
	{
		JCore::Memory::PlacementNew(Cmd);
	}

	SinglePacket(const JCore::MemoryPoolAbstractPtr& allocator, int count)
		: CommandPacket(allocator, 1, sizeof(TCommand))
		, Cmd(*reinterpret_cast<TCommand*>(m_pBuf + PacketHeaderSize_v))
	{
		JCore::Memory::PlacementNew(Cmd);
	}

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_CREATE(int)
	JNET_PACKET_POOLING_RELEASE(SinglePacket)

	WSABUF GetWSABuf() const override {
		*(CmdCnt_t*)(m_pBuf + 0) = m_iCommandCount;
		*(PktLen_t*)(m_pBuf + sizeof(CmdCnt_t)) = m_iPacketLen;

		WSABUF wsaBuf;
		wsaBuf.len = PacketHeaderSize_v + m_iPacketLen;
		wsaBuf.buf = (char*)m_pBuf;
		return wsaBuf;
	}

	int SizeOf() const override { return sizeof(TPacket); }
	char* GetCommandSource() const override { return const_cast<char*>(m_pBuf) + PacketHeaderSize_v; }
	PacketDetailType GetDetailType() const override { return PacketDetailType::SingleStatic; }
	TCommand& Cmd;
private:
	char m_pBuf[PacketHeaderSize_v + sizeof(TCommand)];
};

// 다이나믹 커맨드 전용
template <typename TCommand>
class SinglePacket<TCommand, false> : public CommandPacket {
	static_assert(JCore::IsBaseOf_v<ICommand, TCommand>, "... TCommand must be derived type of \"ICommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
	using TPacket = SinglePacket<TCommand>;
	using TCountableElement = typename TCommand::TCountableElement;
public:
	SinglePacket(int count) : SinglePacket(nullptr, count) {}
	SinglePacket(const JCore::MemoryPoolAbstractPtr& allocator, int count)
		// 실제 보낼 패킷데이터 크기(m_iPacketLen)만큼만 보내기 위해서 m_iPacketLen은 올바르게 설정해줘야한다.
		: CommandPacket(allocator, 1, TCommand::_Size(count)) 
		// count가 0일 경우 구조체 일부가 잘리기 때문에 PlacementNew 수행시 메모리 커럽션이 발생하게 된다. 따라서 생성시에는 count가 0이더라도 1로 가정하고 처리하도록 한다.
		, m_pDynamicBuf(allocator.Exist() ? (char*)allocator->DynamicPop(PacketHeaderSize_v + TCommand::_Size(count <= 0 ? 1 : count)) : dbg_new char[PacketHeaderSize_v + TCommand::_Size(count <= 0 ? 1 : count)])
		, Cmd(*reinterpret_cast<TCommand*>(m_pDynamicBuf + PacketHeaderSize_v))
	{
		// m_pDynamicBuf가 최소 sizeof(TCommand)보다는 커야지 메모리 커럽션이 발생하지 않는다.
		JCore::Memory::PlacementNew(Cmd, count);

		// CountableObject에 대해서 count만큼 생성자 호출을 수행해줘야한다.
		if constexpr (TCommand::ConstructCountableElement) {
			for (int i = 0; i < count - 1; ++i) {
				char* pCountableElementPos = reinterpret_cast<char*>(&Cmd) + sizeof(Cmd) + sizeof(TCountableElement) * i;
				JCore::Memory::PlacementNew(*reinterpret_cast<TCountableElement*>(pCountableElementPos));
			}
		}
	}

	~SinglePacket() override {
		if (m_Allocator.Exist()) {
			m_Allocator->DynamicPush(m_pDynamicBuf, PacketHeaderSize_v + TCommand::_Size(Cmd.Count <= 0 ? 1 : Cmd.Count));
		} else {
			JCORE_DELETE_ARRAY_SAFE(m_pDynamicBuf);
		}
	}

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_CREATE(int)
	JNET_PACKET_POOLING_RELEASE(SinglePacket)

	WSABUF GetWSABuf() const override {
		*(CmdCnt_t*)(m_pDynamicBuf + 0) = m_iCommandCount;
		*(PktLen_t*)(m_pDynamicBuf + sizeof(CmdCnt_t)) = m_iPacketLen;

		WSABUF wsaBuf;
		wsaBuf.len = PacketHeaderSize_v + m_iPacketLen;
		wsaBuf.buf = m_pDynamicBuf;
		return wsaBuf;
	}

	int SizeOf() const override { return sizeof(TPacket); }
	char* GetCommandSource() const override { return const_cast<char*>(m_pDynamicBuf) + PacketHeaderSize_v; }
	PacketDetailType GetDetailType() const override { return PacketDetailType::SingleDynamic; }

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