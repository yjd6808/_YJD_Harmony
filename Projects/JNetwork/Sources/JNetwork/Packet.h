/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/TypeTraits.h>
#include <JCore/RefCount.h>

#include <tuple>

#define PACKET_HEADER_SIZE		4UL		// IPacket  ũ��
#define COMMAND_HEADER_SIZE		4UL		// ICommand ũ��

namespace JNetwork {

/*=====================================================================================
									Ŀ�ǵ� ��ü
						 ��Ŷ�� Ŀ�ǵ带 ��Ƽ� �����Ѵ�.
 =====================================================================================*/


struct ICommand
{
	// ����� ������ �� ������ �� virtual�� ��
	// ���� �Լ� ���̺��� 4����Ʈ�� ����; �� ���ݴ�.
	// 
	// virtual Int16U GetCommand() const = 0;
	// virtual Int16U GetCommandLen() const = 0;

	template <typename T>
	T GetValuePtr() {
		static_assert(JCore::IsPointerType_v<T>, "... T must be pointer type");
		return reinterpret_cast<T>(((char*)this) + sizeof(ICommand));
	}

	void SetCommand(const Int16U cmd)	{ Cmd = cmd; }
	Int16U GetCommand() const			{ return this->Cmd;		}
	Int16U GetCommandLen() const		{ return this->CmdLen;	}
protected:
	Int16U Cmd;		// ����� ���� Ŀ�ǵ� ID��
	Int16U CmdLen;	// Ŀ�ǵ� ���� �̶� CmdLen�� Ŀ�ǵ� ����� ũ�⸦ ���� ������ �����ϵ����Ѵ�.
					// ex) Commnad<char>�� CmdLen�� 1�� �ƴϰ� 5��
};

template <typename T>
struct Command : ICommand 
{ 
	Command() {
		Cmd		= 0;
		CmdLen  = sizeof(Command<T>);		// sizeof(T)�� �� ��� alignment ���� ������ Ŀ�ǵ���̴� T�� ���̱��� �����ؼ� �����ϵ��� ����.
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
								��Ŷ ��ü
		1�� �̻��� Ŀ�ǵ带 ��Ƽ� ������ �� �ֵ����Ѵ�.
		���ø� �Ķ���ͷ� ��� Command�� ��ӹ��� Ÿ���� �����ϵ��� �ؾ��Ѵ�. 
		(�ٸ� Ÿ���� ���� ���ϵ��� ���Ƴ���)

		SendAsync()�� ��Ŷ�� �۽��ϰ� �Ǹ� IOCPOverlappedSend���� �ش� ��Ŷ�� �Ҹ��Ű���� �Ѵ�.

		

		<---------------------- Session Buffer ----------------------------------->
		<--- Packet<Command<A>, Command<B>> ----><---  Packet<Command<C>> --->
		===========================================================================
	   ISendPacket  |  Command<A>  |  Command<B> | ISendPacket | Command<C>  |
		===========================================================================
	    PACKET_HEADER_SIZE      GetPacketLength()
		       ��                        ��
		       4      sizeof(Command<A>) + sizeof(Command<B>)

 =====================================================================================*/



// ��Ŷ�� ���� ���� ���� �Լ� ���̺��� ���� ����ü�� ����.
struct IRecvPacket
{
	Int16U	GetPacketLength() const { return m_iPacketLen;		}
	Int16U	GetCommandCount() const { return m_iCommandCount;	}
protected:
	Int16U m_iCommandCount;
	Int16U m_iPacketLen;		
};


struct ISendPacket : JCore::RefCount
{
	Int16U	GetPacketLength() const { return m_iPacketLen; }
	Int16U	GetCommandCount() const { return m_iCommandCount; }
	WSABUF	GetWSABuf() const { 
		/*
		
		<---------------    Packet<Command<A>, Command<B>> ----------------------->
		<------------------ ISendPacket ---------------- >
		<-- RefCount--> <-------- IRecvPacket -----------> 
		===========================================================================
		 vfptr | m_Ref | m_iCommandCount | m_iPacketCount | Command<A> | Command<B>
		===========================================================================
	    ��              �� <----------------- ����������ϴ� ���� ---------------------->
	   this     this + sizeof(RefCount)

		*/
		WSABUF wsaBuf;
		wsaBuf.len = PACKET_HEADER_SIZE + m_iPacketLen;
		wsaBuf.buf = (char*)this + sizeof(JCore::RefCount);
		return wsaBuf;
	}
protected:
	Int16U m_iCommandCount;
	Int16U m_iPacketLen;		// IPacket ũ�⸦ ������ Ŀ�ǵ���� �� ũ��
								// ICommand�� CmdLen�� ��� ���������� �̳༮�� ���Ծȵ�
};


template <typename... CommandArgs>
class Packet : public ISendPacket
{
	// Ŀ�ǵ�� �ּ� 1�� �̻� �����ϵ��� ����.
	static_assert(sizeof...(CommandArgs) > 0, 
		"... Packet must have one more command");		

	// ���ø� �Ķ���ͷ� ������ ��� Ÿ���� ICommand�� ��ӹ޾ƾ��Ѵ�.
	static_assert(JCore::IsBaseOf_1Base_MultipleDerived_v<ICommand, CommandArgs...>, 
		"... CommandArgs must be derived type of \"ICommand\"");		

	// ���ڷ� ���޹��� Ŀ�ǵ� Ÿ��
	template <int Index>
	using TypeAt		= typename std::tuple_element_t<Index, std::tuple<CommandArgs...>>;	
public:
	Packet() {
		// m_pBuf�� �� Ŀ�ǵ��� �����ּ� ���� ����Ʈ �ʱ�ȭ�� �������ش�.
		// ������ Packet<Commant<A>, Command<B>> ��Ŷ�� �����ߴٸ�
		// 
		// m_pBuf + 0		  ���ٰ� A�� ����Ʈ �ʱ�ȭ�ϰ�
		// m_pBuf + sizeof(A) ���ٰ� B�� ����Ʈ �ʱ�ȭ�ϵ��� �Ѵ�.
		PlacementDefaultAllocateRecursive<COMMAND_CNT - 1, CommandArgs...>();

		this->m_iCommandCount = sizeof...(CommandArgs);
		this->m_iPacketLen = (... + sizeof(CommandArgs));
	}
	virtual ~Packet() {

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

		static_assert(Index < sizeof...(CommandArgs), 
			"... Index must be less than parameter pack count");

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

} // namespace JNetwork