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

#include "jc/Macro.h"
#include "jc/TypeTraits.h"
#include "jc/Primitives/RefCountObjectPtr.h"

#include "WinSock2.h"

#include "jnet/Namespace.h"
#include "jnet/Buffer/PacketBuffer.h"
#include "jnet/Packet/Command.h"

NS_JNET_BEGIN

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

	virtual ~IPacket() override = default;

	virtual	WSABUF			GetWSABuf() const = 0;
	virtual PacketHeader&	GetHeader() const = 0;

	PaloadLen_t		GetPayloadLength() const { return GetHeader().payloadLen_; }
	PacketType_t	GetType() const { return static_cast<PacketType_t>(GetHeader().packetType_); }
	MagicNum_t		GetMagicNumber() const { return GetHeader().magicNumber_; }
	ElemCnt_t		GetElemCount() const { return GetHeader().elemCount_; }

protected:
	jc::MemoryPoolAbstractPtr allocator_; // 소멸 시 반환될 메모리풀
};

//////////////////////////////////////////////////////////////////////////////////////////
class RawPacket : public IPacket
{
	using TPacket = RawPacket;

public:
	RawPacket(char* _pData, int _length)
	{
		jc_assert(_pData != nullptr);
		jc_assert(_length > 0);

		buf_.buf = dbg_new char[PACKET_HEADER_SIZE + _length];
		buf_.len = PACKET_HEADER_SIZE + _length;
		std::memcpy(buf_.buf + PACKET_HEADER_SIZE, _pData, _length);

		Init();
	}

	RawPacket(const jc::MemoryPoolAbstractPtr& _allocator, char* _pData, int _length)
	: IPacket(_allocator)
	{
		jc_assert(_pData != nullptr);
		jc_assert(_length > 0);

		const int totalLen = PACKET_HEADER_SIZE + _length;

		// 여기에서 메모리풀 사용
		buf_.buf = allocator_.Exist()
			? static_cast<char*>(allocator_->DynamicPop(totalLen))
			: dbg_new char[totalLen];
		buf_.len = totalLen;
		std::memcpy(buf_.buf + PACKET_HEADER_SIZE, _pData, _length);

		Init();
	}

	~RawPacket()
	{
		if (allocator_.Exist())
		{
			allocator_->DynamicPush(buf_.buf, static_cast<int>(buf_.len));
		}
		else
		{
			JC_DELETE_ARRAY_SAFE(buf_.buf);
		}
	}

	void Init()
	{
		PacketHeader& header = GetHeader();
		header.magicNumber_ = PACKET_MAGIC_NUMBER;
		header.packetType_ = PacketType::Raw;
		header.payloadLen_ = static_cast<PaloadLen_t>(GetWSABuf().len - PACKET_HEADER_SIZE);
	}

	virtual WSABUF GetWSABuf() const override
	{
		return buf_;
	}

	virtual PacketHeader& GetHeader() const override
	{
		return *(PacketHeader*)buf_.buf;
	}

	JNET_PACKET_POOLING_CREATE(char*, int)
	JNET_PACKET_POOLING_RELEASE(RawPacket)

private:
	WSABUF buf_;
};

//////////////////////////////////////////////////////////////////////////////////////////
class MsgPacket : public IPacket
{

private:

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
   CmdPacket  |  GenericCommand<A>  |  GenericCommand<B> | CmdPacket | GenericCommand<C>  |
	===================================================================================================

 =====================================================================================*/


// 패킷을 받을 때는 가상 함수 테이블이 없는 구조체로 받자.
struct RecvedPacket
{
	RecvedPacket() = delete;
	~RecvedPacket() = delete;

	void ForEach(const jc::Action<ICommand*>& _consumer);
	RecvedPacket* Clone() const; // 삭제 시 필히 Delete 함수를 호출해서 삭제해줄 것.
	void Delete();

	PacketHeader header_;
	char payload_[1];		// raw bytes
};

/*=====================================================================================

		커맨드버퍼 패킷 : 커맨드만 담긴 버퍼를 패킷으로 감싸는 용도 및 커맨드 버퍼 산소호흡기 느낌
		 - 버퍼에 레퍼런스 카운트를 추가하면 구조가 깨짐
		 - 기존 ISendPacket을 활용하고 싶음
		 - 산소호흡기(ref count)를 부착해서 안정적으로 송신하기 위함

  ===================================================================================== */

class PacketBufferPacket : public IPacket
{
	using TPacket = PacketBufferPacket;

public:
	PacketBufferPacket(const PacketBufferPtr& _buffer)
	: pBuffer_(_buffer)
	{
	}

	PacketBufferPacket(const jc::MemoryPoolAbstractPtr& _allocator, const PacketBufferPtr& _buffer)
	: IPacket(_allocator)
	, pBuffer_(_buffer)
	{
	}

	JNET_PACKET_POOLING_CREATE(const PacketBufferPtr&)
	JNET_PACKET_POOLING_RELEASE(PacketBufferPacket)

	virtual WSABUF GetWSABuf() const override
	{
		return { (ULONG)pBuffer_->GetWritePos(), pBuffer_->Source() };
	}

	virtual PacketHeader& GetHeader() const override // 헤더가 여러개 일 수 있다. 일단 맨 앞꺼만..
	{
		return *reinterpret_cast<PacketHeader*>(pBuffer_->Source());
	}
private:
	PacketBufferPtr pBuffer_;
};

/*=====================================================================================
							 스태틱 패킷 : 고정 크기의 커맨드만 담는 녀석
=====================================================================================*/

template <typename... CommandArgs>
class StaticCmdPacket : public IPacket
{
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command");
	static_assert(jc::IsMultipleDerived_v<ICommand, CommandArgs...>,
		"... CommandArgs must be derived type of \"ICommand\"");

	template <int Index>
	using TypeAt = jc::IndexOf_t<Index, CommandArgs...>;
	using TPacket = StaticCmdPacket<CommandArgs...>;

public:
	StaticCmdPacket()
	{
		PlacementDefaultAllocateRecursive<COMMAND_COUNT - 1, CommandArgs...>();
		Init();
	}

	explicit StaticCmdPacket(const jc::MemoryPoolAbstractPtr& _allocator)
	: IPacket(_allocator)
	{
		PlacementDefaultAllocateRecursive<COMMAND_COUNT - 1, CommandArgs...>();
		Init();
	}

	~StaticCmdPacket() override = default;

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_RELEASE(StaticCmdPacket)

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
		return const_cast<char*>(pBuf_) + PACKET_HEADER_SIZE; // 상위 4바이트는 헤더로 사용
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

	void Init()
	{
		PacketHeader& header = GetHeader();
		header.magicNumber_ = PACKET_MAGIC_NUMBER;
		header.packetType_ = PacketType::Command;
		header.payloadLen_ = PACKET_LEN;
		header.elemCount_ = COMMAND_COUNT;

		// 패킷 버퍼는 내부에 메시지/커맨드/로우 등 다양한 패킷이 적재될 수 있음.
		// nothing to do.
	}

	virtual WSABUF GetWSABuf() const override
	{
		return { PACKET_LEN + PACKET_HEADER_SIZE, (char*)pBuf_ };
	}

	virtual PacketHeader& GetHeader() const override // 헤더가 여러개 일 수 있다. 일단 맨 앞꺼만..
	{
		return *(PacketHeader*)pBuf_;
	}

private:
	static constexpr int PACKET_LEN = (... + static_cast<int>(sizeof(CommandArgs)));
	static constexpr int COMMAND_COUNT = sizeof...(CommandArgs);

	char pBuf_[PACKET_LEN + PACKET_HEADER_SIZE]{};
};


/*=====================================================================================

								 따이나믹 패킷 : 가변크기의 커맨드들을 담는 녀석

  TODO: 현재 사용금지, count가 0인 경우에 대해서 Placement New 수행시 메모리 커럽션 발생 문제가 생길 수 있음

  SingleCmdPacket에 대해서는 해당 문제를 수정했으므로 그걸 사용하도록 할 것
  시간이 촉박하고 따이나믹 패킷을 복수로 보내는일은 아직까진 없었으므로.. 수정하진 않는다.
  추후 사용하게 된다면 필히 수정 후 사용할 것!!!
  이걸 해결할려면 다이나믹 커맨드의 CountableObject 배열의 크기를 0으로 설정해놓도록 해야한다.
  ===================================================================================== */

template <typename... CommandArgs>
class DynamicCmdPacket : public IPacket
{
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command");
	static_assert(jc::IsMultipleDerived_v<DynamicCommand, CommandArgs...>,
		"... CommandArgs must be derived type of \"DynamicCommand\"");

	template <int Index>
	using TypeAt = jc::IndexOf_t<Index, CommandArgs...>;
	using TPacket = DynamicCmdPacket<CommandArgs...>;

public:
	// Ints가 모두 정수형일 때만 동작
	template <typename... Ints, typename = jc::DefaultEnableIf_t<jc::And_v<
		jc::IsIntegerType_v<jc::IndexOf_t<0, Ints...>>, jc::IsAllUnaryTrue_v<jc::detail::IsIntegerType, Ints...>>>>
		explicit DynamicCmdPacket(Ints... _counts)
		: DynamicCmdPacket(nullptr, _counts...)
	{
	}

	template <typename TAllocator, typename... Ints, typename = jc::DefaultEnableIf_t<jc::And_v<
		std::is_constructible_v<jc::MemoryPoolAbstractPtr, TAllocator>, jc::IsAllUnaryTrue_v<jc::detail::IsIntegerType, Ints...>>>>
		DynamicCmdPacket(const TAllocator& _nullableAllocator, Ints... _counts)
		: IPacket(_nullableAllocator)
	{
		static_assert(COMMAND_COUNT == sizeof...(_counts), "... Invalid command size count");

		InitializeCountRecursive<0>(_counts...);

		int payloadLen = cmdEndPos_[COMMAND_COUNT];
		if (_nullableAllocator != nullptr)
		{
			if constexpr (!std::is_null_pointer_v<TAllocator>)
			{
				pBuf_ = static_cast<char*>(_nullableAllocator->DynamicPop(PACKET_HEADER_SIZE + payloadLen));
			}
			else
			{
				pBuf_ = dbg_new char[PACKET_HEADER_SIZE + payloadLen];
			}
		}
		else
		{
			pBuf_ = dbg_new char[PACKET_HEADER_SIZE + payloadLen];
		}

		ConstructRecursive<0>(_counts...);

		Init();
	}

	~DynamicCmdPacket() override
	{
		if (allocator_.Exist())
		{
			allocator_->DynamicPush(pBuf_, PACKET_HEADER_SIZE + cmdEndPos_[COMMAND_COUNT]);
		}
		else
		{
			JC_DELETE_ARRAY_SAFE(pBuf_);
		}
	}

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_RELEASE(DynamicCmdPacket)

	template <int Index>
	TypeAt<Index>* Get()
	{
		static_assert(Index < COMMAND_COUNT, "... Index must less than command count");
		return reinterpret_cast<TypeAt<Index>*>(CommandBuf() + cmdEndPos_[Index]);
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
		return pBuf_ + PACKET_HEADER_SIZE;
	}

	void Init()
	{
		PacketHeader& header = GetHeader();
		header.magicNumber_ = PACKET_MAGIC_NUMBER;
		header.packetType_ = PacketType::Command;
		header.payloadLen_ = cmdEndPos_[COMMAND_COUNT];
		header.elemCount_ = COMMAND_COUNT;
	}

	virtual WSABUF GetWSABuf() const override
	{
		return { (ULONG)cmdEndPos_[COMMAND_COUNT] + PACKET_HEADER_SIZE, pBuf_ };
	}

	virtual PacketHeader& GetHeader() const override // 헤더가 여러개 일 수 있다. 일단 맨 앞꺼만..
	{
		return *reinterpret_cast<PacketHeader*>(pBuf_);
	}

private:
	static constexpr int COMMAND_COUNT = sizeof...(CommandArgs);

	int cmdEndPos_[COMMAND_COUNT + 1]{}; // 각 커맨드 길이 임시 기록용
	char* pBuf_{};
};


/*=====================================================================================

							   싱글 패킷 (커맨드 한개만 전송하는 용도)
							   다이나믹, 스태릭 커맨드 아무거나 가능

 TODO[완료]: 다이나믹 버퍼는 메모리풀에서 할당받도록 하는게 좋아보인다.

 ===================================================================================== */
template <typename TCommand, bool IsStatic = TCommand::IS_STATIC>
class SingleCmdPacket;

// 스타틱 커맨드 전용
template <typename TCommand>
class SingleCmdPacket<TCommand, true> : public IPacket
{
	CMD_CHECK_BASE_OF_COMMAND(TCommand)

	using TPacket = SingleCmdPacket<TCommand>;

public:
	// count 매개변수는 다이나믹 커맨드 처리를 위한 특수화 SingleCmdPacket과의 호환성을 위해 둠
	explicit SingleCmdPacket(int _count = 0)
	: cmd_(*(TCommand*)(pBuf_ + PACKET_HEADER_SIZE))
	{
		(void)_count;
		jc::Memory::PlacementNew(cmd_);
		Init();
	}

	SingleCmdPacket(const jc::MemoryPoolAbstractPtr& _allocator, int _count = 0)
	: IPacket(_allocator)
	, cmd_(*(TCommand*)(pBuf_ + PACKET_HEADER_SIZE))
	{
		(void)_count;
		jc::Memory::PlacementNew(cmd_);
		Init();
	}

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_CREATE(int)
	JNET_PACKET_POOLING_RELEASE(SingleCmdPacket)

	void Init()
	{
		PacketHeader& header = GetHeader();
		header.magicNumber_ = PACKET_MAGIC_NUMBER;
		header.packetType_ = PacketType::Command;
		header.payloadLen_ = sizeof(TCommand);
		header.elemCount_ = 1;
	}

	virtual WSABUF GetWSABuf() const override
	{
		return { (ULONG)PACKET_HEADER_SIZE + sizeof(TCommand), (char*)pBuf_ };
	}

	virtual PacketHeader& GetHeader() const override // 헤더가 여러개 일 수 있다. 일단 맨 앞꺼만..
	{
		return *(PacketHeader*)(pBuf_);
	}

	TCommand& cmd_;
private:
	char pBuf_[PACKET_HEADER_SIZE + static_cast<int>(sizeof(TCommand))]{};
};


// 다이나믹 커맨드 전용
template <typename TCommand>
class SingleCmdPacket<TCommand, false> : public IPacket
{
	DYNAMIC_CMD_CHECK_ARRAY_FIELD(TCommand)

	using TPacket = SingleCmdPacket<TCommand>;

public:
	explicit SingleCmdPacket(int _count)
	: SingleCmdPacket(nullptr, _count)
	{
	}

	// m_pDynamicBuf가 최소 sizeof(TCommand)보다는 커야지 메모리 커럽션이 발생하지 않는다.
	// 실제 보낼 패킷데이터 크기(m_iPacketLen)만큼만 보내기 위해서 m_iPacketLen은 올바르게 설정해줘야한다.
	// count가 0일 경우 구조체 일부가 잘리기 때문에 PlacementNew 수행시 메모리 커럽션이 발생하게 된다. 따라서 생성시에는 count가 0이더라도 1로 가정하고 처리하도록 한다.
	//   -> 이제 0으로 사용가능, Flexible Array를 사용하기 약속함
	SingleCmdPacket(const jc::MemoryPoolAbstractPtr& _allocator, int _count)
	: IPacket(_allocator)
	, pBuf_(_allocator.Exist()
		? static_cast<char*>(_allocator->DynamicPop(PACKET_HEADER_SIZE + TCommand::_Size(_count)))
		: dbg_new char[PACKET_HEADER_SIZE + TCommand::_Size(_count)])
	, cmd_(*reinterpret_cast<TCommand*>(pBuf_ + PACKET_HEADER_SIZE))
	{
		// m_pDynamicBuf가 최소 sizeof(TCommand)보다는 커야지 메모리 커럽션이 발생하지 않는다.
		TCommand::_Construct(&cmd_, _count);
		Init();
	}

	~SingleCmdPacket() override
	{
		if (allocator_.Exist())
		{
			allocator_->DynamicPush(pBuf_, PACKET_HEADER_SIZE + TCommand::_Size(cmd_.count_));
		}
		else
		{
			JC_DELETE_ARRAY_SAFE(pBuf_);
		}
	}

	JNET_PACKET_POOLING_CREATE()
	JNET_PACKET_POOLING_CREATE(int)
	JNET_PACKET_POOLING_RELEASE(SingleCmdPacket)

	void Init()
	{
		PacketHeader& header = GetHeader();
		header.magicNumber_ = PACKET_MAGIC_NUMBER;
		header.packetType_ = PacketType::Command;
		header.payloadLen_ = TCommand::_Size(cmd_.count_);
		header.elemCount_ = 1;
	}

	virtual WSABUF GetWSABuf() const override
	{
		return { (ULONG)PACKET_HEADER_SIZE + TCommand::_Size(cmd_.count_), pBuf_ };
	}

	virtual PacketHeader& GetHeader() const override // 헤더가 여러개 일 수 있다. 일단 맨 앞꺼만..
	{
		return *reinterpret_cast<PacketHeader*>(pBuf_);
	}

	// @참고: https://stackoverflow.com/questions/2669888/initialization-order-of-class-data-members
	// 클래스 필드는 배열한 순서대로 초기화가 이뤄진다.
	// 따라서 참조커맨드는 무조건 다이나믹 버퍼가 초기화된 후 초기화해줘야한다.
	char* pBuf_;
	TCommand& cmd_;
};


using ISendPacketPtr = jc::SharedPtr<IPacket>;
using ISendPacketGuard = jc::RefCountObjectPtr<IPacket>;

#define JNET_SEND_PACKET_AUTO_RELEASE_GUARD(packet) ISendPacketGuard JC_CONCAT_COUNTER(__autorelease_guard__)(packet, false)

NS_END
