/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:24:50 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/TypeTraits.h>

#include <JNetwork/Namespace.h>


/*=====================================================================================
								 커맨드 객체
					  패킷에 커맨드를 담아서 전송한다.
 =====================================================================================*/

// 패킷 헤더
using CmdCnt_t  = Int16U;
using PktLen_t  = Int16U;

// 커맨드 헤더
using CmdType_t = Int8U;
using Cmd_t		= Int16U;
using CmdLen_t	= Int16U;

#define CMD_ALIGNMENT 4

NS_JNET_BEGIN

inline constexpr int PacketHeaderSize_v = sizeof(CmdCnt_t) + sizeof(PktLen_t);
inline constexpr int CommandHeaderSize_v = sizeof(CmdType_t) + sizeof(Cmd_t) + sizeof(CmdLen_t);

#pragma pack(push, CMD_ALIGNMENT)

struct CmdType
{
	enum
	{
		Generic = 1,
		Static,
		Dynamic,
		Max
	};
};

struct ICommand
{
	void SetCommand(const Cmd_t cmd) { Cmd = cmd; }
	void SetCommandLen(const CmdLen_t cmdlen) { CmdLen = cmdlen; }
	void AddCommandLen(const CmdLen_t cmdlen) { CmdLen += cmdlen; }
	Cmd_t GetCommand() const { return this->Cmd; }
	char* GetData() const { return (char*)this + sizeof(ICommand); }
	int GetDataLen() const { return CmdLen - sizeof(ICommand); }
	CmdLen_t GetCommandLen() const { return this->CmdLen; }
	bool IsExtraCmdType() const { return Type > (int)CmdType::Dynamic; }

	// 전체 캐스팅 - 상속받은 커스텀 커맨드 전용
	template <typename T>
	T CastCommand() {
		static_assert(JCore::IsPointerType_v<T>, "... T must be pointer command type");
		return reinterpret_cast<T>(this);
	}

	// 뒷부분만 캐스팅 - GenericCommand<T> 전용
	template <typename T>
	T CastValue() {
		static_assert(JCore::IsPointerType_v<T>, "... T must be pointer type");
		return reinterpret_cast<T>(((char*)this) + sizeof(ICommand));
	}

	
	CmdType_t Type;
	Cmd_t	 Cmd;		// 사용자 지정 커맨드 ID값
	CmdLen_t CmdLen;	// 커맨드 길이 이때 CmdLen은 커맨드 헤더의 크기를 더한 값으로 설정하도록한다.
	// ex) Commnad<char>의 CmdLen은 1이 아니고 5임
};

// 쓸일은 없겟지만 테스트용도
template <typename T>
struct GenericCommand : ICommand
{
	GenericCommand() {
		Type = CmdType::Generic;
		Cmd = 0;
		CmdLen = sizeof(GenericCommand<T>);		// sizeof(T)로 할 경우 alignment 문제 때문에 커맨드길이는 T의 길이까지 포함해서 전송하도록 하자.
		Value = T();
	}
	GenericCommand(const Int16U cmd) {
		Type = CmdType::Generic;
		Cmd = cmd;
		CmdLen = sizeof(GenericCommand<T>);
		Value = T();
	}

	T Value;
};

struct Staticity { static constexpr bool IsStatic = true; };
struct Dynamicity { static constexpr bool IsDynamic = true; };
struct DynamicCommandBase { int Count{}; };

template <int CmdType>
struct Command
{
	static constexpr bool IsValid = false;
	static constexpr bool IsDynamic = false;
	static constexpr bool IsStatic = false;
};

template <>
struct Command<CmdType::Static> : ICommand
{
	static constexpr bool IsValid = true;
	static constexpr bool IsDynamic = false;
	static constexpr bool IsStatic = true;
};

template <>
struct Command<CmdType::Dynamic> : ICommand, DynamicCommandBase
{
	static constexpr bool IsValid = true;
	static constexpr bool IsDynamic = true;
	static constexpr bool IsStatic = false;
};




using StaticCommand = Command<CmdType::Static>;
using DynamicCommand = Command<CmdType::Dynamic>;

	NS_DETAIL_BEGIN
	template<typename T>
	struct IsStaticCommand : JCore::IntegralConstant<bool, JCore::IsBaseOf_v<Staticity, T>> {};
	template<typename T>
	struct IsDynamicCommand : JCore::IntegralConstant<bool, JCore::IsBaseOf_v<Dynamicity, T>> {};
	NS_DETAIL_END

template <typename T>
constexpr bool IsStaticCommand_v = Detail::IsStaticCommand<T>::Value;

template <typename T>
constexpr bool IsDynamicCommand_v = Detail::IsDynamicCommand<T>::Value;


NS_JNET_END

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)

enum CmdDirection
{
	eNone = 0b0000,
	eClientToServer = 0b0001,
	eServerToClient = 0b0010,
	eServerToServer = 0b0100,
	eClientToClient = 0b1000,
	eAll = eClientToServer | eServerToClient | eServerToServer | eClientToClient
};

inline static constexpr const char* CmdDirectionName[1 << 4] = {
	"None",
	"ClientToServer",
	"ServerToClient",
	"ServerToClient | ClientToServer",
	"ServerToServer",
	"ServerToServer | ClientToServer",
	"ServerToServer | ServerToClient",
	"ServerToServer | ServerToClient | ClientToServer",
	"ClientToClient",
	"ClientToClient | ClientToServer",
	"ClientToClient | ServerToClient",
	"ClientToClient | ServerToClient | ClientToServer",
	"ClientToClient | ServerToServer",
	"ClientToClient | ServerToServer | ClientToServer",
	"ClientToClient | ServerToServer | ServerToClient",
	"ClientToClient | ServerToServer | ServerToClient | ClientToServer",
};

/*=====================================================================================
								 커맨드 생성 규칙
					  아래 규칙에 맞게 커맨드를 생성토록 한다.
 =====================================================================================*/

#define STATIC_CMD_BEGIN(__struct__, __cmd__, __cmd_direction__)				\
struct __struct__ : JNetwork::StaticCommand {									\
	__struct__(int count = 1) {													\
		Type = JNetwork::CmdType::Static;										\
		Cmd = __cmd__;															\
		CmdLen = sizeof(__struct__);											\
	}																			\
																				\
	static constexpr int Size(int count = 1) { return sizeof(__struct__); }		\
	static constexpr const char* Name() { return #__struct__; }					\
	static constexpr int Command() { return __cmd__; }							\
	static constexpr int Direction() { return __cmd_direction__; }
	

#define STATIC_CMD_END(__struct__) };


// @https://stackoverflow.com/questions/35196871/what-is-the-optimal-order-of-members-in-a-class
#define DYNAMIC_CMD_BEGIN(__struct__, __cmd__, __cmd_direction__, __countable_elem_type__)									\
struct __struct__ : JNetwork::DynamicCommand {																				\
	__struct__(int count) {																									\
		Type = JNetwork::CmdType::Dynamic;																					\
		Cmd = __cmd__;																										\
		CmdLen = sizeof(__struct__) + sizeof(__countable_elem_type__ ) * (count - 1);										\
		Count = count;																										\
	}																														\
	static constexpr int Size(int count) { return sizeof(__struct__) + sizeof(__countable_elem_type__ ) * (count - 1);}		\
	static constexpr const char* Name() { return #__struct__; }																\
	static constexpr int Command() { return __cmd__; }																		\
	static constexpr int Direction() { return __cmd_direction__; }

#define DYNAMIC_CMD_END(__struct__)	};
