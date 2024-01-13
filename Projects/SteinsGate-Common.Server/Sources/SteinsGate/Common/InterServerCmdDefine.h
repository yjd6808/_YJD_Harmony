/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 4:52:19 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>
#include <JNetwork/Packet/Command.h>
#include <SteinsGate/Common/Const.h>


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
struct Command<InterServerCmdType::HostStatic> : ICommand, Staticity
{
	static constexpr bool IsValid = true;
	static constexpr bool IsRelay = false;
	static constexpr bool IsHost = true;
};

template <>
struct Command<InterServerCmdType::HostDynamic> : ICommand, Dynamicity, DynamicCommandBase
{
	static constexpr bool IsValid = true;
	static constexpr bool IsRelay = false;
	static constexpr bool IsHost = true;
};

template <>
struct Command<InterServerCmdType::RelayStatic> : ICommand, Staticity, RelayCommandBase
{
	static constexpr bool IsValid = true;
	static constexpr bool IsRelay = true;
	static constexpr bool IsHost = false;
};

template <>
struct Command<InterServerCmdType::RelayDynamic> : ICommand, Dynamicity, RelayCommandBase, DynamicCommandBase
{
	static constexpr bool IsValid = true;
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

#define HOST_DYNAMIC_CMD_ADD_ZERO_SIZE_ARRAY_FIELD(struct)		DYNAMIC_CMD_ADD_ZERO_SIZE_ARRAY_FIELD(struct)
#define RELAY_DYNAMIC_CMD_ADD_ZERO_SIZE_ARRAY_FIELD(struct)		DYNAMIC_CMD_ADD_ZERO_SIZE_ARRAY_FIELD(struct)

#define HOST_STATIC_CMD_BEGIN(__struct__, __cmd__)												\
struct __struct__ : HostStaticCommand {															\
	__struct__(int count = 1) {																	\
		Type = InterServerCmdType::HostStatic;													\
		Cmd = __cmd__;																			\
		CmdLen = sizeof(__struct__);															\
	}																							\
																								\
	CMD_FUNC_DEF_NAME(__struct__)																\
	CMD_FUNC_DEF_COMMAND(__cmd__)																\
	CMD_FUNC_DEF_SIZE(__struct__)																\
	CMD_FUNC_DEF_CONSTRUCT(__struct__)

#define HOST_STATIC_CMD_END };


#define HOST_DYNAMIC_CMD_BEGIN(...)		JCORE_CONCAT_ARGS(HOST_DYNAMIC_CMD_BEGIN_IMPL_, JCORE_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)
#define HOST_DYNAMIC_CMD_BEGIN_IMPL_3(__struct__, __cmd__, __countable_elem_type__) HOST_DYNAMIC_CMD_BEGIN_IMPL_4(__struct__, __cmd__, __countable_elem_type__, true)
#define HOST_DYNAMIC_CMD_BEGIN_IMPL_4(__struct__, __cmd__, __countable_elem_type__, __construct_countable_elem__)	\
struct __struct__ : HostDynamicCommand {																			\
	DYNAMIC_CMD_USING_COUNTABLE_ELEMENT(__countable_elem_type__)													\
	DYNAMIC_CMD_DECL_COUNTABLE_ELEMENT(__construct_countable_elem__)												\
																													\
	__struct__(int count) {																							\
		Type = InterServerCmdType::HostDynamic;																		\
		Cmd = __cmd__;																								\
		CmdLen = _Size(count);																						\
		Count = count;																								\
	}																												\
	CMD_FUNC_DEF_NAME(__struct__)																					\
	CMD_FUNC_DEF_COMMAND(__cmd__)																					\
	CMD_FUNC_DEF_SIZE(__struct__, __countable_elem_type__)															\
	CMD_FUNC_DEF_CONSTRUCT(__struct__, __countable_elem_type__)																			

#define HOST_DYNAMIC_CMD_END	};


#define RELAY_STATIC_CMD_BEGIN(__struct__, __cmd__)						\
struct __struct__ : RelayStaticCommand {								\
	__struct__(int count = 1) {											\
		Type = InterServerCmdType::RelayStatic;							\
		Cmd = __cmd__;													\
		CmdLen = sizeof(__struct__);									\
	}																	\
																		\
	CMD_FUNC_DEF_NAME(__struct__)										\
	CMD_FUNC_DEF_COMMAND(__cmd__)										\
	CMD_FUNC_DEF_SIZE(__struct__)										\
	CMD_FUNC_DEF_CONSTRUCT(__struct__)									

#define RELAY_STATIC_CMD_END };


#define RELAY_DYNAMIC_CMD_BEGIN(...)		JCORE_CONCAT_ARGS(RELAY_DYNAMIC_CMD_BEGIN_IMPL_, JCORE_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)
#define RELAY_DYNAMIC_CMD_BEGIN_IMPL_3(__struct__, __cmd__, __countable_elem_type__) RELAY_DYNAMIC_CMD_BEGIN_IMPL_4(__struct__, __cmd__, __countable_elem_type__, true)
#define RELAY_DYNAMIC_CMD_BEGIN_IMPL_4(__struct__, __cmd__, __countable_elem_type__, __construct_countable_elem__)	\
struct __struct__ : RelayDynamicCommand {																	\
	DYNAMIC_CMD_USING_COUNTABLE_ELEMENT(__countable_elem_type__)											\
	DYNAMIC_CMD_DECL_COUNTABLE_ELEMENT(__construct_countable_elem__)										\
																											\
	__struct__(int count) {																					\
		Type = InterServerCmdType::RelayDynamic;															\
		Cmd = __cmd__;																						\
		CmdLen = _Size(count);																				\
		Count = count;																						\
	}																										\
	CMD_FUNC_DEF_NAME(__struct__)																			\
	CMD_FUNC_DEF_COMMAND(__cmd__)																			\
	CMD_FUNC_DEF_SIZE(__struct__, __countable_elem_type__)													\
	CMD_FUNC_DEF_CONSTRUCT(__struct__, __countable_elem_type__)

#define RELAY_DYNAMIC_CMD_END	};