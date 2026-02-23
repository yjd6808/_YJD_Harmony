/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:23:36 PM
 * =====================
 * 송신용 패킷 (4타입)
 *   - 멀티 스타릭 패킷 (스타릭 커맨드 여러개 담아서 전송)
 *	 - 멀티 다이나믹 패킷 (다이나믹 커맨드 여러개 담아서 전송)
 *	 - 커맨드 버퍼 패킷 (커맨드 버퍼 통째로 전송)
 *	 - 싱글 패킷 (스타릭 커맨드, 다이나믹 커맨드 암거나 1개만 담음)
 *
 *	 굳이 멀티 패킷을 정의하지 않고 멀티 스타릭, 멀티 다이나믹으로 구분한 이유는
 *	 스타릭커맨드는 크기가 컴파일타임에 결정되므로 멀티 스타릭 패킷은 패킷을 한번 생성하는 것만으로 모든게 종견된다.
 *	 다이나믹커맨드는 크기가 런타임에 결정되므로 멀티 다이나믹 패킷은 패킷을 한번 생성하고 내부에서 한번더 런타임에 결정된 크기대로 동적할당하여 커맨드 데이터를 생성해줘야한다.
 */


#pragma once

#include <jc/Macro.h>
#include <jc/TypeTraits.h>
#include <jc/Primitives/RefCountObjectPtr.h>

#include <WinSock2.h>

#include <jnet/Namespace.h>
#include <jnet/Buffer/CommandBuffer.h>
#include <jnet/Packet/Command.h>

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

// 매크로 처리로 인해 라인 디버깅이 힘들 수 있음
#define JNET_PACKET_POOLING_PARAMS_0()                          const jc::MemoryPoolAbstractPtr& allocator
#define JNET_PACKET_POOLING_PARAMS_1(argty1)                    const jc::MemoryPoolAbstractPtr& allocator, argty1 arg1
#define JNET_PACKET_POOLING_PARAMS_2(argty1, argty2)            const jc::MemoryPoolAbstractPtr& allocator, argty1 arg1, argty2 arg2
#define JNET_PACKET_POOLING_PARAMS_3(argty1, argty2, argty3)    const jc::MemoryPoolAbstractPtr& allocator, argty1 arg1, argty2 arg2, argty3 arg3

#define JNET_PACKET_POOLING_ARGS_0()
#define JNET_PACKET_POOLING_ARGS_1(argty1)                      arg1
#define JNET_PACKET_POOLING_ARGS_2(argty1, argty2)              arg1, arg2
#define JNET_PACKET_POOLING_ARGS_3(argty1, argty2, argty3)      arg1, arg2, arg3

#define JNET_PACKET_POOLING_ARGS_WITH_PACKET_0()                            pPacket, allocator
#define JNET_PACKET_POOLING_ARGS_WITH_PACKET_1(argty1)                      pPacket, allocator, arg1
#define JNET_PACKET_POOLING_ARGS_WITH_PACKET_2(argty1, argty2)              pPacket, allocator, arg1, arg2
#define JNET_PACKET_POOLING_ARGS_WITH_PACKET_3(argty1, argty2, argty3)      pPacket, allocator, arg1, arg2, arg3

#define JNET_PACKET_POOLING_PARAMS(...)                         JC_EXPAND_1(JC_CONCAT_2(JNET_PACKET_POOLING_PARAMS_, JC_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define JNET_PACKET_POOLING_ARGS(...)                           JC_EXPAND_1(JC_CONCAT_2(JNET_PACKET_POOLING_ARGS_,   JC_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define JNET_PACKET_POOLING_ARGS_WITH_PACKET(...)               JC_EXPAND_1(JC_CONCAT_2(JNET_PACKET_POOLING_ARGS_WITH_PACKET_, JC_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))

#define JNET_PACKET_POOLING_CREATE(...)                                                                 \
	static TPacket* Create(JNET_PACKET_POOLING_PARAMS(__VA_ARGS__))                                      \
	{                                                                                                    \
		if (allocator == nullptr)                                                                        \
		{                                                                                                \
			return dbg_new TPacket(JNET_PACKET_POOLING_ARGS(__VA_ARGS__));                                \
		}                                                                                                \
		int unusedSize = 0;                                                                             \
		TPacket* pPacket = static_cast<TPacket*>(allocator->DynamicPop(sizeof(TPacket), unusedSize));   \
		jc::Memory::PlacementNew(JNET_PACKET_POOLING_ARGS_WITH_PACKET(__VA_ARGS__));                  \
		return pPacket;                                                                                  \
	}

// spGuard를 둔 이유: allocator의 레퍼런스 카운트가 1남은 경우 패킷 소멸자 호출시 allocator가 소멸되어버리는 문제가 발생해버릴 수 있다.
// 따라서 레퍼런스 카운트를 1올려놓은 후 패킷 반환이 이뤄진 이후에 allocator의 ref count를 낮추도록 한다.
// allocator가 없는 경우는 dbg_new TPacket과 같이 동적할당해서 생성해버린 경우이거나 스택할당해서 생성해버린 경우이다.
// TODO: 어떻게하면 spGuard를 사용하지 않을 수 있을까?
//  -> 이 풀이 네트워킹동안 절대 소멸되지 않음을 보장해줘야한다
//  -> 즉, 개발자가 풀의 LifeCycle을 관리해줘야한다는 말이다. ㅠㅠ
//  -> 그러면 개발자가 LifeCycle을 관리하지 않을 수는 없을까? ... 이건 최후의 최적화 방법으로 남겨놓도록 하자.
#define JNET_PACKET_POOLING_RELEASE(class_name)                                      \
	void ReleaseAction() override                                                     \
	{                                                                                \
		if (!allocator_.Exist())                                                      \
		{                                                                            \
			delete this;                                                              \
			return;                                                                   \
		}                                                                            \
		jc::MemoryPoolAbstractPtr spGuard = allocator_;                            \
		jc::MemoryPoolAbstract* pAllocator = allocator_.GetPtr();                  \
		this->~class_name();                                                         \
		pAllocator->DynamicPush(this, sizeof(TPacket));                              \
	}


class IPacket : public jc::RefCountObject
{
public:
	IPacket() = default;

	explicit IPacket(const jc::MemoryPoolAbstractPtr& _allocator)
	: allocator_(_allocator)
	{
	}

	~IPacket() override = default;

	virtual WSABUF GetWSABuf() const = 0;
	virtual PacketType GetType() const = 0;
	virtual PacketDetailType GetDetailType() const = 0;
	virtual int SizeOf() const = 0;

protected:
	jc::MemoryPoolAbstractPtr allocator_; // 소멸 시 반환될 메모리풀
};


class RawPacket : public IPacket
{
	using TPacket = RawPacket;

public:
	RawPacket(char* _pData, int _length)
	{
		buf_.buf = _pData;
		buf_.len = _length;
	}

	RawPacket(const jc::MemoryPoolAbstractPtr& _allocator, char* _pData, int _length)
	: IPacket(_allocator)
	{
		buf_.buf = _pData;
		buf_.len = _length;
	}

	JNET_PACKET_POOLING_CREATE(char*, int)
	JNET_PACKET_POOLING_RELEASE(RawPacket)

	WSABUF GetWSABuf() const override
	{
		return buf_;
	}

	PacketType GetType() const override
	{
		return PacketType::Raw;
	}

	PacketDetailType GetDetailType() const override
	{
		return PacketDetailType::Raw;
	}

	int SizeOf() const override
	{
		return sizeof(TPacket);
	}

private:
	WSABUF buf_{};
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

	explicit CommandPacket(const jc::MemoryPoolAbstractPtr& _allocator)
	: IPacket(_allocator)
	{
	}

	CommandPacket(CmdCnt_t _commandCount, PktLen_t _packetLength)
	: commandCount_(_commandCount)
	, packetLength_(_packetLength)
	{
	}

	CommandPacket(const jc::MemoryPoolAbstractPtr& _allocator, CmdCnt_t _commandCount, PktLen_t _packetLength)
	: IPacket(_allocator)
	, commandCount_(_commandCount)
	, packetLength_(_packetLength)
	{
	}

	~CommandPacket() override = default;

	PktLen_t GetPacketLength() const
	{
		return packetLength_;
	}

	CmdCnt_t GetCommandCount() const
	{
		return commandCount_;
	}

	virtual char* GetCommandSource() const = 0; // 커맨드 시작위치 반환

	ICommand* GetCommand(int _index)
	{
		if (_index >= commandCount_ || _index < 0)
		{
			return nullptr;
		}

		ICommand* pCommand = nullptr;
		char* pCommandData = GetCommandSource();
		int index = 0;

		while (index < commandCount_)
		{
			pCommand = reinterpret_cast<ICommand*>(pCommandData);

			if (index == _index)
			{
				break;
			}

			pCommandData += pCommand->GetLength();
			++index;
		}

		return pCommand;
	}

	void ForEach(const jc::Action<ICommand*>& _consumer)
	{
		int commandIndex = 0;
		char* pCommandData = GetCommandSource();

		while (commandIndex < commandCount_)
		{
			ICommand* pCurrentCommand = reinterpret_cast<ICommand*>(pCommandData);
			_consumer(pCurrentCommand);

			if (pCurrentCommand->GetLength() >= 2500)
			{
				jc_assert(false);
				break;
			}

			pCommandData += pCurrentCommand->GetLength();
			++commandIndex;
		}
	}

	PacketType GetType() const override
	{
		return PacketType::Command;
	}

protected:
	CmdCnt_t commandCount_{};
	PktLen_t packetLength_{}; /// IPacket 크기를 제외한 커맨드들의 총 크기
	// ICommand의 CmdLen은 헤더 포함이지만 이녀석은 포함안됨
};


// 패킷을 받을 때는 가상 함수 테이블이 없는 구조체로 받자.
class RecvedCommandPacket
{
public:
	RecvedCommandPacket() = delete;
	~RecvedCommandPacket() = delete;

	_u16 GetPacketLength() const
	{
		return packetLength_;
	}

	_u16 GetCommandCount() const
	{
		return commandCount_;
	}

	void ForEach(const jc::Action<ICommand*>& _consumer);

	// 삭제 시 필히 char*로 캐스팅 후 delete[] 해줄 것
	RecvedCommandPacket* Clone() const;

protected:
	_u16 commandCount_{};
	_u16 packetLength_{};
};


/*=====================================================================================
							 스태틱 패킷 : 고정 크기의 커맨드만 담는 녀석
=====================================================================================*/

template <typename... CommandArgs>
class StaticPacket : public CommandPacket
{
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command");
	static_assert(jc::IsMultipleDerived_v<ICommand, CommandArgs...>,
	              "... CommandArgs must be derived type of \"ICommand\"");

	template <int Index>
	using TypeAt = jc::IndexOf_t<Index, CommandArgs...>;
	using TPacket = StaticPacket<CommandArgs...>;

public:
	StaticPacket()
	: CommandPacket(COMMAND_COUNT, PACKET_LEN)
	{
		PlacementDefaultAllocateRecursive<COMMAND_COUNT - 1, CommandArgs...>();
	}

	explicit StaticPacket(const jc::MemoryPoolAbstractPtr& _allocator)
	: CommandPacket(_allocator, COMMAND_COUNT, PACKET_LEN)
	{
		PlacementDefaultAllocateRecursive<COMMAND_COUNT - 1, CommandArgs...>();
	}

	~StaticPacket() override = default;

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_RELEASE(StaticPacket)

	WSABUF GetWSABuf() const override 
	{
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
		*(CmdCnt_t*)(buffer_ + 0) = commandCount_;
		*(PktLen_t*)(buffer_ + sizeof(CmdCnt_t)) = packetLength_;

		WSABUF wsaBuf;
		wsaBuf.len = PACKET_HEADER_SIZE + packetLength_;
		wsaBuf.buf = (char*)buffer_;
		return wsaBuf;
	}

	char* GetCommandSource() const override
	{
		return const_cast<char*>(buffer_) + PACKET_HEADER_SIZE;
	}

	PacketDetailType GetDetailType() const override
	{
		return PacketDetailType::Static;
	}

private:
	template <int Index, typename Cmd, typename... CmdArgs>
	constexpr void PlacementDefaultAllocateRecursive()
	{
		if constexpr (Index == 0)
		{
			::new(Get<Index>()) TypeAt<Index>();
		}
		else
		{
			::new(Get<Index>()) TypeAt<Index>();
			PlacementDefaultAllocateRecursive<Index - 1, CmdArgs...>();
		}
	}

	template <int Index>
	constexpr int SumOfSizeRecursive()
	{
		if constexpr (Index == 0)
		{
			return sizeof(TypeAt<0>);
		}
		else
		{
			return static_cast<int>(sizeof(TypeAt<Index>)) + SumOfSizeRecursive<Index - 1>();
		}
	}

	char* CommandBuf() const
	{
		return const_cast<char*>(buffer_) + PACKET_HEADER_SIZE; // 상위 4바이트는 헤더로 사용
	}

public:
	template <int Index>
	constexpr TypeAt<Index>* Get()
	{
		static_assert(Index < sizeof...(CommandArgs), "... Index must be less than parameter pack count");

		if constexpr (Index == 0)
		{
			return reinterpret_cast<TypeAt<Index>*>(CommandBuf());
		}
		else
		{
			return reinterpret_cast<TypeAt<Index>*>(CommandBuf() + SumOfSizeRecursive<Index - 1>());
		}
	}

	int SizeOf() const override
	{
		return sizeof(TPacket);
	}

private:
	static constexpr int PACKET_LEN = (... + static_cast<int>(sizeof(CommandArgs)));
	static constexpr int COMMAND_COUNT = sizeof...(CommandArgs);

	char buffer_[PACKET_LEN + PACKET_HEADER_SIZE]{};
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
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command");
	static_assert(jc::IsMultipleDerived_v<DynamicCommand, CommandArgs...>,
	              "... CommandArgs must be derived type of \"DynamicCommand\"");

	template <int Index>
	using TypeAt = jc::IndexOf_t<Index, CommandArgs...>;
	using TPacket = DynamicPacket<CommandArgs...>;

public:
	// Ints가 모두 정수형일 때만 동작
	template <typename... Ints, typename = jc::DefaultEnableIf_t<jc::And_v<
		jc::IsIntegerType_v<jc::IndexOf_t<0, Ints...>>, jc::IsAllUnaryTrue_v<jc::detail::IsIntegerType, Ints...>>>>
	explicit DynamicPacket(Ints... _counts)
	: DynamicPacket(nullptr, _counts...)
	{
	}

	template <typename TAllocator, typename... Ints, typename = jc::DefaultEnableIf_t<jc::And_v<
		          std::is_constructible_v<jc::MemoryPoolAbstractPtr, TAllocator>, jc::IsAllUnaryTrue_v<jc::detail::IsIntegerType, Ints...>>>>
	DynamicPacket(const TAllocator& _nullableAllocator, Ints... _counts)
	: CommandPacket(_nullableAllocator)
	{
		static_assert(COMMAND_COUNT == sizeof...(_counts), "... Invalid command size count");

		InitializeCountRecursive<0>(_counts...);

		packetLength_ = cmdEndPos_[COMMAND_COUNT];
		commandCount_ = COMMAND_COUNT;

		if (_nullableAllocator != nullptr)
		{
			if constexpr (!std::is_null_pointer_v<TAllocator>)
			{
				dynamicBuf_ = static_cast<char*>(_nullableAllocator->DynamicPop(PACKET_HEADER_SIZE + packetLength_));
			}
			else
			{
				dynamicBuf_ = dbg_new char[PACKET_HEADER_SIZE + packetLength_];
			}
		}
		else
		{
			dynamicBuf_ = dbg_new char[PACKET_HEADER_SIZE + packetLength_];
		}

		ConstructRecursive<0>(_counts...);
	}

	~DynamicPacket() override
	{
		if (allocator_.Exist())
		{
			allocator_->DynamicPush(dynamicBuf_, PACKET_HEADER_SIZE + packetLength_);
		}
		else
		{
			JC_DELETE_ARRAY_SAFE(dynamicBuf_);
		}
	}

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_RELEASE(DynamicPacket)

	WSABUF GetWSABuf() const override
	{
		*reinterpret_cast<CmdCnt_t*>(dynamicBuf_ + 0) = commandCount_;
		*reinterpret_cast<PktLen_t*>(dynamicBuf_ + sizeof(CmdCnt_t)) = packetLength_;

		WSABUF wsaBuf;
		wsaBuf.len = PACKET_HEADER_SIZE + packetLength_;
		wsaBuf.buf = dynamicBuf_;
		return wsaBuf;
	}

	char* GetCommandSource() const override
	{
		return dynamicBuf_ + PACKET_HEADER_SIZE;
	}

	template <int Index>
	TypeAt<Index>* Get()
	{
		static_assert(Index < COMMAND_COUNT, "... Index must less than command count");
		return reinterpret_cast<TypeAt<Index>*>(CommandBuf() + cmdEndPos_[Index]);
	}

	int SizeOf() const override
	{
		return sizeof(TPacket);
	}

	PacketDetailType GetDetailType() const override
	{
		return PacketDetailType::Dynamic;
	}

private:
	template <int Index>
	void InitializeCountRecursive(int _count)
	{
		int size = TypeAt<Index>::_Size(_count);

		if constexpr (Index > 0)
		{
			size += cmdEndPos_[Index];
		}

		cmdEndPos_[Index + 1] = size;
	}

	template <int Index, typename... Ints>
	void InitializeCountRecursive(int _count, Ints... _counts)
	{
		int size = TypeAt<Index>::_Size(_count);

		if constexpr (Index > 0)
		{
			size += cmdEndPos_[Index];
		}

		cmdEndPos_[Index + 1] = size;
		InitializeCountRecursive<Index + 1>(_counts...);
	}

	template <int Index>
	void ConstructRecursive(int _count)
	{
		using TCommand = TypeAt<Index>;

		TCommand* pCommand = Get<Index>();
		TCommand::_Construct(pCommand, _count);
	}

	template <int Index, typename... Ints>
	void ConstructRecursive(int _count, Ints... _counts)
	{
		using TCommand = TypeAt<Index>;

		TCommand* pCommand = Get<Index>();
		TCommand::_Construct(pCommand, _count);
		ConstructRecursive<Index + 1>(_counts...);
	}

	char* CommandBuf() const
	{
		return dynamicBuf_ + PACKET_HEADER_SIZE;
	}

private:
	static constexpr int COMMAND_COUNT = sizeof...(CommandArgs);

	int cmdEndPos_[COMMAND_COUNT + 1]{}; // 각 커맨드 길이 임시 기록용
	char* dynamicBuf_{};
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
	explicit CommandBufferPacket(const CommandBufferPtr& _buffer);
	CommandBufferPacket(const jc::MemoryPoolAbstractPtr& _allocator, const CommandBufferPtr& _buffer);

	JNET_PACKET_POOLING_CREATE(const CommandBufferPtr&)
	JNET_PACKET_POOLING_RELEASE(CommandBufferPacket)

	WSABUF GetWSABuf() const override
	{
		return {static_cast<ULONG>(buffer_->GetWritePos()), buffer_->Source()};
	}

	char* GetCommandSource() const override
	{
		return buffer_->Source() + PACKET_HEADER_SIZE;
	}

	PacketDetailType GetDetailType() const override
	{
		return PacketDetailType::CommandBuffer;
	}

	int SizeOf() const override
	{
		return sizeof(TPacket);
	}

private:
	CommandBufferPtr buffer_;
};


/*=====================================================================================

							   싱글 패킷 (커맨드 한개만 전송하는 용도)
							   다이나믹, 스태릭 커맨드 아무거나 가능

 TODO[완료]: 다이나믹 버퍼는 메모리풀에서 할당받도록 하는게 좋아보인다.

 ===================================================================================== */
template <typename TCommand, bool IsStatic = TCommand::IS_STATIC>
class SinglePacket;

// 스타틱 커맨드 전용
template <typename TCommand>
class SinglePacket<TCommand, true> : public CommandPacket
{
	CMD_CHECK_BASE_OF_COMMAND(TCommand)

	using TPacket = SinglePacket<TCommand>;

public:
	// count 매개변수는 다이나믹 커맨드 처리를 위한 특수화 SinglePacket과의 호환성을 위해 둠
	explicit SinglePacket(int _count = 0)
	: CommandPacket(1, static_cast<PktLen_t>(sizeof(TCommand)))
	, cmd_(*reinterpret_cast<TCommand*>(buffer_ + PACKET_HEADER_SIZE))
	{
		(void)_count;
		jc::Memory::PlacementNew(cmd_);
	}

	SinglePacket(const jc::MemoryPoolAbstractPtr& _allocator, int _count)
	: CommandPacket(_allocator, 1, static_cast<PktLen_t>(sizeof(TCommand)))
	, cmd_(*reinterpret_cast<TCommand*>(buffer_ + PACKET_HEADER_SIZE))
	{
		(void)_count;
		jc::Memory::PlacementNew(cmd_);
	}

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_CREATE(int)
	JNET_PACKET_POOLING_RELEASE(SinglePacket)

	WSABUF GetWSABuf() const override
	{
		*reinterpret_cast<CmdCnt_t*>((char*)buffer_ + 0) = commandCount_;
		*reinterpret_cast<PktLen_t*>((char*)buffer_ + sizeof(CmdCnt_t)) = packetLength_;

		WSABUF wsaBuf;
		wsaBuf.len = PACKET_HEADER_SIZE + packetLength_;
		wsaBuf.buf = (char*)buffer_;
		return wsaBuf;
	}

	int SizeOf() const override
	{
		return sizeof(TPacket);
	}

	char* GetCommandSource() const override
	{
		return const_cast<char*>(buffer_) + PACKET_HEADER_SIZE;
	}

	PacketDetailType GetDetailType() const override
	{
		return PacketDetailType::SingleStatic;
	}

	TCommand& cmd_;

private:
	char buffer_[PACKET_HEADER_SIZE + static_cast<int>(sizeof(TCommand))]{};
};


// 다이나믹 커맨드 전용
template <typename TCommand>
class SinglePacket<TCommand, false> : public CommandPacket
{
	DYNAMIC_CMD_CHECK_ARRAY_FIELD(TCommand)

	using TPacket = SinglePacket<TCommand>;

public:
	explicit SinglePacket(int _count)
	: SinglePacket(nullptr, _count)
	{
	}
	
	// m_pDynamicBuf가 최소 sizeof(TCommand)보다는 커야지 메모리 커럽션이 발생하지 않는다.
	// 실제 보낼 패킷데이터 크기(m_iPacketLen)만큼만 보내기 위해서 m_iPacketLen은 올바르게 설정해줘야한다.
	// count가 0일 경우 구조체 일부가 잘리기 때문에 PlacementNew 수행시 메모리 커럽션이 발생하게 된다. 따라서 생성시에는 count가 0이더라도 1로 가정하고 처리하도록 한다.
	//   -> 이제 0으로 사용가능, Flexible Array를 사용하기 약속함
	SinglePacket(const jc::MemoryPoolAbstractPtr& _allocator, int _count)
	: CommandPacket(_allocator, 1, static_cast<PktLen_t>(TCommand::_Size(_count)))
	, pDynamicBuf_(_allocator.Exist()
		              ? static_cast<char*>(_allocator->DynamicPop(PACKET_HEADER_SIZE + TCommand::_Size(_count)))
		              : dbg_new char[PACKET_HEADER_SIZE + TCommand::_Size(_count)])
	, cmd_(*reinterpret_cast<TCommand*>(pDynamicBuf_ + PACKET_HEADER_SIZE))
	{
		// m_pDynamicBuf가 최소 sizeof(TCommand)보다는 커야지 메모리 커럽션이 발생하지 않는다.
		TCommand::_Construct(&cmd_, _count);
	}

	~SinglePacket() override
	{
		if (allocator_.Exist())
		{
			allocator_->DynamicPush(pDynamicBuf_, PACKET_HEADER_SIZE + TCommand::_Size(cmd_.count_));
		}
		else
		{
			JC_DELETE_ARRAY_SAFE(pDynamicBuf_);
		}
	}

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_CREATE(int)
	JNET_PACKET_POOLING_RELEASE(SinglePacket)

	WSABUF GetWSABuf() const override
	{
		*reinterpret_cast<CmdCnt_t*>(pDynamicBuf_ + 0) = commandCount_;
		*reinterpret_cast<PktLen_t*>(pDynamicBuf_ + sizeof(CmdCnt_t)) = packetLength_;

		WSABUF wsaBuf;
		wsaBuf.len = PACKET_HEADER_SIZE + packetLength_;
		wsaBuf.buf = pDynamicBuf_;
		return wsaBuf;
	}

	int SizeOf() const override
	{
		return sizeof(TPacket);
	}

	char* GetCommandSource() const override
	{
		return pDynamicBuf_ + PACKET_HEADER_SIZE;
	}

	PacketDetailType GetDetailType() const override
	{
		return PacketDetailType::SingleDynamic;
	}

	// @참고: https://stackoverflow.com/questions/2669888/initialization-order-of-class-data-members
	// 클래스 필드는 배열한 순서대로 초기화가 이뤄진다.
	// 따라서 참조커맨드는 무조건 다이나믹 버퍼가 초기화된 후 초기화해줘야한다.
	char* pDynamicBuf_;
	TCommand& cmd_;
};


using ISendPacketPtr = jc::SharedPtr<IPacket>;
using ISendPacketGuard = jc::RefCountObjectPtr<IPacket>;

#define JNET_SEND_PACKET_AUTO_RELEASE_GUARD(packet) ISendPacketGuard JC_CONCAT_COUNTER(__autorelease_guard__)(packet, false)

NS_END
