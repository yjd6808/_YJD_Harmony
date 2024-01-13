/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:24:50 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Macro.h>
#include <JCore/Memory.h>
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

	// 삭제시 필히 char*로 캐스팅 후 delete[] 해줄 것
	ICommand* Clone() const;
	
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

struct Staticity
{
	static constexpr bool IsStatic = true;
	static constexpr bool IsDyamic = false;
};

struct Dynamicity
{
	static constexpr bool IsStatic = false;
	static constexpr bool IsDynamic = true;
};

struct DynamicCommandBase { int Count{}; };

template <int CmdType>
struct Command
{
	static constexpr bool IsValid = false;
	static constexpr bool IsDynamic = false;
	static constexpr bool IsStatic = false;
};

template <>
struct Command<CmdType::Static> : ICommand, Staticity
{
	static constexpr bool IsValid = true;
};

template <>
struct Command<CmdType::Dynamic> : ICommand, Dynamicity, DynamicCommandBase
{
	static constexpr bool IsValid = true;
};




using StaticCommand = Command<CmdType::Static>;
using DynamicCommand = Command<CmdType::Dynamic>;

	NS_DETAIL_BEGIN
	template<typename T>
	struct IsStaticCommand : JCore::IntegralConstant<bool, JCore::IsBaseOf_v<Staticity, T>> {};
	template<typename T>
	struct IsDynamicCommand : JCore::IntegralConstant<bool, JCore::IsBaseOf_v<Dynamicity, T>> {};

	template<typename TCommand>
	struct HasDynamicFiled
	{
		template <typename = JCore::DefaultEnableIf_t<std::is_union_v<TCommand::__ZeroSizeArray__>>>
		static constexpr bool Check(int*) { return true; }
		static constexpr bool Check(...) { return false; }
		static constexpr bool Value = Check(nullptr);
	};
	NS_DETAIL_END

template <typename T>
constexpr bool IsStaticCommand_v = Detail::IsStaticCommand<T>::Value;

template <typename T>
constexpr bool IsDynamicCommand_v = Detail::IsDynamicCommand<T>::Value;

template <typename TCommand>
constexpr bool HasDynamicField_v = Detail::HasDynamicFiled<TCommand>::Value;


NS_JNET_END

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)


/*=====================================================================================
								 커맨드 생성 규칙
					  아래 규칙에 맞게 커맨드를 생성토록 한다.
 =====================================================================================*/


// 올바른 커맨드인지 체크하는 기능
// 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
#define CMD_CHECK_BASE_OF_COMMAND(command)					static_assert(JCore::IsBaseOf_v<JNetwork::ICommand, command>, "... " #command " must be derived type of ICommand"); 
// 스타릭 커맨드는 무조건 통과, 다이나믹 커맨드일 경우 다이나믹 필드가 있는지 체크
#define DYNAMIC_CMD_CHECK_ZERO_SIZE_ARRAY_FIELD(command)	static_assert(JCore::Or_v<JNetwork::IsStaticCommand_v<command>, JCore::And_v<JNetwork::IsDynamicCommand_v<command>, JNetwork::HasDynamicField_v<command>>>, "... " #command " has no zero size array field");


// 스타릭 커맨드, 다이나믹 커맨드 공통 함수 정의
// count = 0으로 둔 이유는 static cmmand와 dynamic command간 상호호환을 위해서이다.
#define CMD_FUNC_DEF_NAME(struct)		static constexpr const char* _Name() { return #struct; }
#define CMD_FUNC_DEF_COMMAND(cmd)		static constexpr int _Command() { return cmd; }
#define CMD_FUNC_DEF_SIZE(...) 			JCORE_CONCAT_ARGS(CMD_FUNC_DEF_SIZE_, JCORE_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)
#define CMD_FUNC_DEF_SIZE_1(struct)		static constexpr int _Size(int count = 0) { return sizeof(struct); } // static command 전용
#define CMD_FUNC_DEF_SIZE_2(struct, countable_elem_type) static constexpr int _Size(int count) { return sizeof(struct) + sizeof(countable_elem_type) * count; }		// dynamic command 전용
#define CMD_FUNC_DEF_CONSTRUCT(...)		JCORE_CONCAT_ARGS(CMD_FUNC_DEF_CONSTRUCT_, JCORE_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)
#define CMD_FUNC_DEF_CONSTRUCT_1(struct)	static void _Construct(void* p, int count = 0) { return JCore::Memory::PlacementNew(*static_cast<struct*>(p)); }
#define CMD_FUNC_DEF_CONSTRUCT_2(struct, countable_elem_type) static void _Construct(void* p, int count) {		\
	JCore::Memory::PlacementNew(*static_cast<struct*>(p), count);															\
	if constexpr (ConstructCountableElement) {																				\
		for (int i = 0; i < count; ++i) {																					\
			void* pCountableElementPos = static_cast<char*>(p) + sizeof(struct) + sizeof(countable_elem_type) * i;			\
			JCore::Memory::PlacementNew(*static_cast<TCountableElement*>(pCountableElementPos));							\
		}																													\
	}																														\
}


// 다이나믹 커맨드 공통 기능 정의
#define DYNAMIC_CMD_USING_COUNTABLE_ELEMENT(element_type)			using TCountableElement = element_type;
#define DYNAMIC_CMD_DECL_COUNTABLE_ELEMENT(contruct_countable_elem)	static constexpr bool ConstructCountableElement = contruct_countable_elem;
#define DYNAMIC_CMD_ADD_ZERO_SIZE_ARRAY_FIELD(elem_name)	\
union __ZeroSizeArray__ {									\
	__ZeroSizeArray__() {}									\
	~__ZeroSizeArray__() {}									\
	TCountableElement elem_name;							\
} Object[0];												\
															\
TCountableElement* elem_name() {							\
	if (Count <= 0) {										\
		return nullptr;										\
	}														\
	return &Object[0].##elem_name;							\
}															\
															\
TCountableElement& elem_name##At(int idx) {					\
	if (idx < 0 || idx >= Count) {							\
		DebugAssertMsg(false, "%s 커맨드의 다이나믹 필드 갯수가 %d인데 %d인덱스에 접근을 시도했습니다.", _Name(), Count, idx); \
		throw JCore::OutOfRangeException("");				\
	}														\
	return Object[idx].##elem_name;							\
}

#define STATIC_CMD_BEGIN(__struct__, __cmd__)						\
struct __struct__ : JNetwork::StaticCommand {						\
	__struct__(int count = 0) {										\
		Type = JNetwork::CmdType::Static;							\
		Cmd = __cmd__;												\
		CmdLen = sizeof(__struct__);								\
	}																\
																	\
	CMD_FUNC_DEF_NAME(__struct__)									\
	CMD_FUNC_DEF_COMMAND(__cmd__)									\
	CMD_FUNC_DEF_SIZE(__struct__)									\
	CMD_FUNC_DEF_CONSTRUCT(__struct__)								

#define STATIC_CMD_END };


// @https://stackoverflow.com/questions/35196871/what-is-the-optimal-order-of-members-in-a-class
#define DYNAMIC_CMD_BEGIN(...)		JCORE_CONCAT_ARGS(DYNAMIC_CMD_BEGIN_IMPL_, JCORE_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)
#define DYNAMIC_CMD_BEGIN_IMPL_3(__struct__, __cmd__, __countable_elem_type__) DYNAMIC_CMD_BEGIN_IMPL_4(__struct__, __cmd__, __countable_elem_type__, true)
#define DYNAMIC_CMD_BEGIN_IMPL_4(__struct__, __cmd__, __countable_elem_type__, __construct_countable_elem__)	\
struct __struct__ : JNetwork::DynamicCommand {																	\
	DYNAMIC_CMD_USING_COUNTABLE_ELEMENT(__countable_elem_type__)												\
	DYNAMIC_CMD_DECL_COUNTABLE_ELEMENT(__construct_countable_elem__)											\
																												\
	__struct__(int count) {																						\
		Type = JNetwork::CmdType::Dynamic;																		\
		Cmd = __cmd__;																							\
		CmdLen = _Size(count);																					\
		Count = count;																							\
	}																											\
																												\
	CMD_FUNC_DEF_NAME(__struct__)																				\
	CMD_FUNC_DEF_COMMAND(__cmd__)																				\
	CMD_FUNC_DEF_SIZE(__struct__, __countable_elem_type__)														\
	CMD_FUNC_DEF_CONSTRUCT(__struct__, __countable_elem_type__)													

#define DYNAMIC_CMD_END	};
