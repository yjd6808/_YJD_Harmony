/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/TypeTraits.h>

#include <tuple>

namespace JNetwork {

/*=====================================================================================
									커맨드 객체
						 패킷에 커맨드를 담아서 전송한다.
 =====================================================================================*/

struct ICommand
{
	Int16U Cmd;
	Int16U CmdLen;
};

template <typename T>
struct Command : ICommand { 
	Command(const Int16U cmd) {
		Cmd		= cmd;
		CmdLen	= sizeof(T);
	}

	T Value;
};


/*=====================================================================================
								패킷 객체
					1개 이상의 커맨드를 담아서 전송할 수 있도록한다.
					템플릿 파라미터로 모두 Command를 상속받은 타입을 전달하도록 해야한다. 
					(다른 타입은 전달 못하도록 막아놓음)

					SendAsync()로 패킷을 송신하게 되면 IOCPOverlappedSend에서 해당 패킷을 소멸시키도록 한다.
 =====================================================================================*/

struct IPacket
{
	virtual char*	GetBuf() const = 0;
	virtual Int32U	GetPacketLength() const = 0;
	virtual Int32U	GetCommandCount() const = 0;
	virtual WSABUF	GetWSABuf() const = 0;
	virtual void	Release() { delete this; }
};

template <typename... Types>
class PacketHeader : public IPacket
{
public:
	PacketHeader() {
		m_iCommandCount = sizeof...(Types);
		m_iPacketLen = (... + sizeof(Types));
	}
	virtual ~PacketHeader() {}
public:
	virtual Int32U GetPacketLength() const { return m_iPacketLen;		}
	virtual Int32U GetCommandCount() const { return m_iCommandCount;	}
protected:
	Int32U m_iCommandCount;
	Int32U m_iPacketLen;

	static constexpr int PACKET_LEN = (... + sizeof(Types));
};



template <typename... CommandArgs>
class Packet : public PacketHeader<CommandArgs...>
{
	// 커맨드는 최소 1개 이상 전달하도록 하자.
	static_assert(sizeof...(CommandArgs) > 0, 
		"... Packet must have one more command");		

	// 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
	static_assert(JCore::IsBaseOf_1Base_MultipleDerived_v<ICommand, CommandArgs...>, 
		"... CommandArgs must be derived type of \"ICommand\"");		

	// 인자로 전달받은 커맨드 타입
	template <int Index>
	using TypeAt		= typename std::tuple_element_t<Index, std::tuple<CommandArgs...>>;	
	using TPacketHeader = typename PacketHeader<CommandArgs...>;
public:
	Packet() : TPacketHeader(), m_pBuf("") {}
	virtual ~Packet() {}
public:
	template <int Index>
	constexpr int SumOfSize() {
		if constexpr (Index == 0) {
			return sizeof(TypeAt<0>);
		} else {
			return sizeof(TypeAt<Index>) + SumOfSize<Index - 1>();
		}
	}

	template <int Index>
	constexpr TypeAt<Index>* Get() {

		static_assert(Index < sizeof...(CommandArgs), 
			"... Index must be less than parameter pack count");

		if constexpr (Index == 0) {
			return (TypeAt<Index>*)m_pBuf;
		} else {
			return (TypeAt<Index>*)(m_pBuf + SumOfSize<Index - 1>());
		}
	}

	virtual char*  GetBuf() const { return (char*)m_pBuf; }
	virtual WSABUF GetWSABuf() const { return { this->GetPacketLength(), GetBuf() }; }

private:
	char m_pBuf[TPacketHeader::PACKET_LEN];
};

} // namespace JNetwork