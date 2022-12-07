/*
 *	작성자 : 윤정도
 *
 *	ISendPacket은 생성시 바로 Packet에서 멤버 초기화를 진행해주므로 해당 인스펙션은 무시하도록하자.
 */

#pragma once

#include <JCore/Type.h>
#include <JCore/Define.h>
#include <JCore/TypeTraits.h>
#include <JCore/SafeRefCount.h>

#include <tuple>
#include <WinSock2.h>

#define PACKET_HEADER_SIZE		4UL		// IPacket  크기
#define COMMAND_HEADER_SIZE		4UL		// ICommand 크기

namespace JNetwork {

/*=====================================================================================
									커맨드 객체
						 패킷에 커맨드를 담아서 전송한다.
 =====================================================================================*/


struct ICommand
{
	// 가상 함수 테이블땜에 4바이트씩 먹음; 걍 없앴다.
	// virtual Int16U GetCommand() const = 0;
	// virtual Int16U GetCommandLen() const = 0;

	void SetCommand(const Int16U cmd)			{ Cmd = cmd;			}
	void SetCommandLen(const Int16U cmdlen)		{ CmdLen = cmdlen;		}
	void AddCommandLen(const Int16U cmdlen)		{ CmdLen += cmdlen;		}
	Int16U GetCommand() const					{ return this->Cmd;		}
	Int16U GetCommandLen() const				{ return this->CmdLen;	}

	// ICommand 통틀어서 캐스팅 - ICommand를 상속받은 커스텀 커맨드 전용
	template <typename T>
	T CastCommand() {
		static_assert(JCore::IsPointerType_v<T>, "... T must be pointer command type");
		return reinterpret_cast<T>(this);
	}

	// ICommandㄷ를 제외하고 뒷부분만 캐스팅 - Command<T> 전용
	template <typename T>
	T CastValue() {
		static_assert(JCore::IsPointerType_v<T>, "... T must be pointer type");
		return reinterpret_cast<T>(((char*)this) + sizeof(ICommand));
	}

protected:
	Int16U Cmd{};		// 사용자 지정 커맨드 ID값
	Int16U CmdLen{};	// 커맨드 길이 이때 CmdLen은 커맨드 헤더의 크기를 더한 값으로 설정하도록한다.
						// ex) Commnad<char>의 CmdLen은 1이 아니고 5임
};

template <typename T>
struct Command : ICommand 
{ 
	Command() {
		Cmd		= 0;
		CmdLen  = sizeof(Command<T>);		// sizeof(T)로 할 경우 alignment 문제 때문에 커맨드길이는 T의 길이까지 포함해서 전송하도록 하자.
		Value   = T();
	}
	Command(const Int16U cmd) {
		Cmd		= cmd;
		CmdLen	= sizeof(Command<T>);
		Value   = T();
	}

	T Value;
};



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



// 패킷을 받을 때는 가상 함수 테이블이 없는 구조체로 받자.
struct IRecvPacket
{
	IRecvPacket() = delete;
	~IRecvPacket() = delete;

	Int16U	GetPacketLength() const { return m_iPacketLen;		}
	Int16U	GetCommandCount() const { return m_iCommandCount;	}
protected:
	Int16U m_iCommandCount;
	Int16U m_iPacketLen;		
};


struct ISendPacket : JCore::SafeRefCount
{
	ISendPacket() {}
	ISendPacket(Int16U iCommandCount, Int16U iPacketLen)
		: m_iCommandCount(iCommandCount)
		, m_iPacketLen(iPacketLen) {
	}

	~ISendPacket() override = default;

	Int16U	GetPacketLength() const { return m_iPacketLen; }
	Int16U	GetCommandCount() const { return m_iCommandCount; }
	virtual WSABUF GetWSABuf() const = 0;
    void ReleaseAction() override { delete this; }
protected:
	Int16U m_iCommandCount{};
	Int16U m_iPacketLen{};		// IPacket 크기를 제외한 커맨드들의 총 크기
								// ICommand의 CmdLen은 헤더 포함이지만 이녀석은 포함안됨
};


template <typename... CommandArgs>
class StaticPacket : public ISendPacket
{
	static_assert(sizeof...(CommandArgs) > 0,  "... Packet must have one more command"); // 커맨드는 최소 1개 이상 전달하도록 하자.
	static_assert(JCore::IsBaseOf_1Base_MultipleDerived_v<ICommand, CommandArgs...>,  "... CommandArgs must be derived type of \"ICommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.

	template <int Index>
	using TypeAt = std::tuple_element_t<Index, std::tuple<CommandArgs...>>;	 // 인자로 전달받은 커맨드 타입
public:
	StaticPacket() : ISendPacket(COMMAND_CNT, PACKET_LEN) {

		// m_pBuf에 각 커맨드의 시작주소 마다 디폴트 초기화를 수행해준다.
		// 예를들어서 Packet<Commant<A>, Command<B>> 패킷을 생성했다면
		// 
		// m_pBuf + 0		  에다가 A를 디폴트 초기화하고
		// m_pBuf + sizeof(A) 에다가 B를 디폴트 초기화하도록 한다.
		PlacementDefaultAllocateRecursive<COMMAND_CNT - 1, CommandArgs...>();
	}

	~StaticPacket() override = default;

	WSABUF GetWSABuf() const override {
		/*

		<---------------    StaticPacket<Command<A>, Command<B>> ----------------------->
		<------------------ ISendPacket ---------------- >
		<-- RefCount--> <-------- IRecvPacket ----------->
		===========================================================================
		 vfptr | m_Ref | m_iCommandCount | m_iPacketCount | Command<A> | Command<B>
		===========================================================================
		↑              ↑ <----------------- 전송해줘야하는 구간 ---------------------->
	   this     this + sizeof(RefCount)

		*/
		WSABUF wsaBuf;
		wsaBuf.len = PACKET_HEADER_SIZE + m_iPacketLen;
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
	static constexpr int PACKET_LEN = (... + sizeof(CommandArgs));
	static constexpr int COMMAND_CNT = sizeof...(CommandArgs);

	char m_pBuf[PACKET_LEN];
};


template <typename... CommandArgs>
class DynamicPacket : public ISendPacket
{
	static_assert(sizeof...(CommandArgs) > 0, "... Packet must have one more command"); // 커맨드는 최소 1개 이상 전달하도록 하자.
	static_assert(JCore::IsBaseOf_1Base_MultipleDerived_v<ICommand, CommandArgs...>, "... CommandArgs must be derived type of \"ICommand\""); // 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.

	template <int Index>
	using TypeAt = std::tuple_element_t<Index, std::tuple<CommandArgs...>>;	 // 인자로 전달받은 커맨드 타입
public:
	template <typename... Ints>
	DynamicPacket(Ints... sizes) : ISendPacket() {
		static_assert(COMMAND_CNT == sizeof...(sizes), "... Invalid command size count");
		const int iPacketLen = (... + sizes);
		this->m_iPacketLen = iPacketLen;
		this->m_iCommandCount = COMMAND_CNT;
		this->m_pDynamicBuf = new char[PACKET_HEADER_SIZE + iPacketLen];

		InitializeCommandLenRecursive<0>(sizes...);
	}
	~DynamicPacket() override {
		DeleteArraySafe(m_pDynamicBuf);
	}


	WSABUF GetWSABuf() const override {
		// 패킷 상위 4바이트는 패킷 헤더로 사용한다.
		*(Int16U*)(m_pDynamicBuf + 0) = m_iCommandCount;
		*(Int16U*)(m_pDynamicBuf + sizeof(Int16U)) = m_iPacketLen;

		WSABUF wsaBuf;
		wsaBuf.len = PACKET_HEADER_SIZE + m_iPacketLen;
		wsaBuf.buf = m_pDynamicBuf;
		return wsaBuf;
	}

	template <int Index, typename... Args>
	void Construct(Args&&... args) {
		static_assert(Index < COMMAND_CNT, "... Index must less then command count");
		auto pCmd = ::new (CommandBuf() + CommandPos(Index)) TypeAt<Index>{ JCore::Forward<Args>(args)... };
		pCmd->SetCommandLen(m_pCommandSizes[Index]);
	}

	template <int Index>
	TypeAt<Index>* Get() {
		static_assert(Index < COMMAND_CNT, "... Index must less then command count");
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
		return m_pDynamicBuf + PACKET_HEADER_SIZE; // m_pDynamicBuf의 상위 4바이트는 헤더로 사용하기 때문에
	}

	
private:
	static constexpr int COMMAND_CNT = sizeof...(CommandArgs);

	int m_pCommandSizes[COMMAND_CNT]{};		// 각 커맨드 길이 임시 기록용
	char* m_pDynamicBuf;
};


} // namespace JNetwork