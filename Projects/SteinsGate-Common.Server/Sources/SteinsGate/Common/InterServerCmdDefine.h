/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 4:52:19 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/Command.h>
#include <SteinsGate/Common/Config.h>
#include <SteinsGate/Common/EnumDefine.h>

struct InterServerCmdType : JNetwork::CmdType
{
	enum _
	{
		Generic = 1,
		Begin = Generic,
		Static,
		Dynamic,
		HostStatic,									// 중앙 서버 <-> 개별서버간 통신시의 스타릭 커맨드
		InterServerBegin = HostStatic,
		HostDynamic,								// 중앙 서버 <-> 개별서버간 통신시의 다나믹 커맨드
		RelayStatic,								// 중앙 서버를 거쳐서 통신하는 스타릭 커맨드
		RelayDynamic,								// 중앙 서버를 거쳐서 통신하는 다나믹 커맨드
		End = RelayDynamic,
		InterServerEnd = RelayDynamic,
		Max
	};
};

#pragma pack(push, CMD_ALIGNMENT)

NS_JNET_BEGIN

struct RelayCommandBase { Int8 From = -1; Int8 To = -1; };

template <>
struct Command<InterServerCmdType::HostStatic> : ICommand
{
	static constexpr bool IsValid = true;
	static constexpr bool IsDynamic = false;
	static constexpr bool IsStatic = true;
	static constexpr bool IsRelay = false;
	static constexpr bool IsHost = true;
};

template <>
struct Command<InterServerCmdType::HostDynamic> : ICommand, DynamicCommandBase
{
	static constexpr bool IsValid = true;
	static constexpr bool IsDynamic = false;
	static constexpr bool IsStatic = true;
	static constexpr bool IsRelay = false;
	static constexpr bool IsHost = true;
};

template <>
struct Command<InterServerCmdType::RelayStatic> : ICommand, RelayCommandBase
{
	static constexpr bool IsValid = true;
	static constexpr bool IsDynamic = false;
	static constexpr bool IsStatic = true;
	static constexpr bool IsRelay = true;
	static constexpr bool IsHost = false;
};

template <>
struct Command<InterServerCmdType::RelayDynamic> : ICommand, DynamicCommandBase, RelayCommandBase
{
	static constexpr bool IsValid = true;
	static constexpr bool IsDynamic = false;
	static constexpr bool IsStatic = true;
	static constexpr bool IsRelay = true;
	static constexpr bool IsHost = false;
};

NS_JNET_END


#pragma pack(pop)


using HostStaticCommand = JNetwork::Command<InterServerCmdType::HostStatic>;
using HostDynamicCommand = JNetwork::Command<InterServerCmdType::HostDynamic>;
using RelayStaticCommand = JNetwork::Command<InterServerCmdType::RelayStatic>;
using RelayDynamicCommand = JNetwork::Command<InterServerCmdType::RelayDynamic>;

template<typename T>
struct IsInterServerCommand : JCore::IntegralConstant<bool,
	JCore::Or_v<
		JCore::IsBaseOf_v<HostStaticCommand, T>,
		JCore::IsBaseOf_v<HostDynamicCommand, T>,
		JCore::IsBaseOf_v<RelayStaticCommand, T>,
		JCore::IsBaseOf_v<RelayDynamicCommand, T>
	>
> {};

template<typename T>
struct IsInterServerHostCommand : JCore::IntegralConstant<bool,
	JCore::Or_v<
		JCore::IsBaseOf_v<HostStaticCommand, T>,
		JCore::IsBaseOf_v<HostDynamicCommand, T>
	>
> {};

template<typename T>
struct IsInterServerRelayCommand : JCore::IntegralConstant<bool,
	JCore::Or_v<
		JCore::IsBaseOf_v<RelayStaticCommand, T>,
		JCore::IsBaseOf_v<RelayDynamicCommand, T>
	>
> {};

template<typename T> constexpr bool IsInterServerCommand_v = IsInterServerCommand<T>::Value;
template<typename T> constexpr bool IsInterServerHostCommand_v = IsInterServerHostCommand<T>::Value;
template<typename T> constexpr bool IsInterServerRelayCommand_v = IsInterServerRelayCommand<T>::Value;


enum InterServerCmdDirection
{
	eAuthToCenter	= 0b00000000000001,	
	eAuthToLobby	= 0b00000000000010,
	eAuthToGame		= 0b00000000000100,
	eCenterToGame	= 0b00000000001000,
	eCenterToLobby	= 0b00000000010000,
	eCenterToAuth	= 0b00000000100000,
	eLobbyToCenter	= 0b00000001000000,
	eLobbyToAuth	= 0b00000010000000,
	eLobbyToGame	= 0b00000100000000,
	eGameToCenter	= 0b00001000000000,
	eGameToAuth	 	= 0b00010000000000,
	eGameToLobby	= 0b00100000000000,
	ePeerToCenter	= 0b01000000000000,	// 개별 서버 모두가 중앙서버로
	eCenterToPeer	= 0b10000000000000, // 중앙 서버가 다른 모든 서버로
};

#define HOST_STATIC_CMD_BEGIN(__struct__, __cmd__, __cmd_direction__)							\
struct __struct__ : HostStaticCommand {															\
	__struct__(int count = 1) {																	\
		Type = InterServerCmdType::HostStatic;													\
		Cmd = __cmd__;																			\
		CmdLen = sizeof(__struct__);															\
	}																							\
																								\
	static constexpr int Size(int count = 1) { return sizeof(__struct__); }						\
	static constexpr const char* Name() { return #__struct__; }									\
	static constexpr int Command() { return __cmd__; }											\
	static constexpr int Direction() { return __cmd_direction__; }
#define HOST_STATIC_CMD_END(__struct__) };


#define HOST_DYNAMIC_CMD_BEGIN(__struct__, __cmd__, __cmd_direction__, __countable_elem_type__)								\
struct __struct__ : HostDynamicCommand {																					\
	__struct__(int count) {																									\
		Type = InterServerCmdType::HostDynamic;																				\
		Cmd = __cmd__;																										\
		CmdLen = sizeof(__struct__) + sizeof(__countable_elem_type__ ) * (count - 1);										\
		Count = count;																										\
	}																														\
	static constexpr int Size(int count) { return sizeof(__struct__) + sizeof(__countable_elem_type__ ) * (count - 1);}		\
	static constexpr const char* Name() { return #__struct__; }																\
	static constexpr int Command() { return __cmd__; }																		\
	static constexpr int Direction() { return __cmd_direction__; }

#define HOST_DYNAMIC_CMD_END(__struct__)	};


#define RELAY_STATIC_CMD_BEGIN(__struct__, __cmd__, __cmd_direction__)							\
struct __struct__ : RelayStaticCommand {														\
	__struct__(int count = 1) {																	\
		Type = InterServerCmdType::RelayStatic;													\
		Cmd = __cmd__;																			\
		CmdLen = sizeof(__struct__);															\
	}																							\
																								\
	static constexpr int Size(int count = 1) { return sizeof(__struct__); }						\
	static constexpr const char* Name() { return #__struct__; }									\
	static constexpr int Command() { return __cmd__; }											\
	static constexpr int Direction() { return __cmd_direction__; }
#define HOST_STATIC_CMD_END(__struct__) };


#define RELAY_DYNAMIC_CMD_BEGIN(__struct__, __cmd__, __cmd_direction__, __countable_elem_type__)							\
struct __struct__ : RelayDynamicCommand {																					\
	__struct__(int count) {																									\
		Type = InterServerCmdType::RelayDynamic;																			\
		Cmd = __cmd__;																										\
		CmdLen = sizeof(__struct__) + sizeof(__countable_elem_type__ ) * (count - 1);										\
		Count = count;																										\
	}																														\
	static constexpr int Size(int count) { return sizeof(__struct__) + sizeof(__countable_elem_type__ ) * (count - 1);}		\
	static constexpr const char* Name() { return #__struct__; }																\
	static constexpr int Command() { return __cmd__; }																		\
	static constexpr int Direction() { return __cmd_direction__; }

#define HOST_DYNAMIC_CMD_END(__struct__)	};