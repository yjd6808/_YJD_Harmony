/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:24:50 PM
 * =====================
 *
 */


#pragma once

#include <jc/Type.h>
#include <jc/Macro.h>
#include <jc/Memory.h>
#include <jc/TypeTraits.h>

#include <jnet/Namespace.h>


/*=====================================================================================
								 커맨드 객체
					  패킷에 커맨드를 담아서 전송한다.
=====================================================================================*/

// 패킷 헤더
using CmdCnt_t = Int16U;
using PktLen_t = Int16U;

// 커맨드 헤더
using CmdType_t = Int8U;
using Cmd_t = Int16U;
using CmdLen_t = Int16U;

#define CMD_ALIGNMENT 4

NS_JNET_BEGIN

inline constexpr int PACKET_HEADER_SIZE = sizeof(CmdCnt_t) + sizeof(PktLen_t);
inline constexpr int COMMAND_HEADER_SIZE = sizeof(CmdType_t) + sizeof(Cmd_t) + sizeof(CmdLen_t);

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

class ICommand
{
public:
	CmdType_t GetType() const
	{
		return type_;
	}

	void SetId(const Cmd_t _cmdId)
	{
		cmdId_ = _cmdId;
	}

	void SetLength(const CmdLen_t _cmdLength)
	{
		cmdLength_ = _cmdLength;
	}

	void AddLength(const CmdLen_t _cmdLength)
	{
		cmdLength_ += _cmdLength;
	}

	Cmd_t GetId() const
	{
		return cmdId_;
	}

	char* GetData() const
	{
		return reinterpret_cast<char*>(const_cast<ICommand*>(this)) + sizeof(ICommand);
	}

	int GetDataLength() const
	{
		return cmdLength_ - static_cast<CmdLen_t>(sizeof(ICommand));
	}

	CmdLen_t GetLength() const
	{
		return cmdLength_;
	}

	bool IsExtraCmdType() const
	{
		return static_cast<int>(type_) > static_cast<int>(CmdType::Dynamic);
	}

	// 전체 캐스팅 - 상속받은 커스텀 커맨드 전용
	template <typename T>
	T CastCommand()
	{
		static_assert(jc::IsPointerType_v<T>, "... T must be pointer command type");
		return reinterpret_cast<T>(this);
	}

	// 뒷부분만 캐스팅 - GenericCommand<T> 전용
	template <typename T>
	T CastValue()
	{
		static_assert(jc::IsPointerType_v<T>, "... T must be pointer type");
		return reinterpret_cast<T>(GetData());
	}

	// 삭제시 필히 char*로 캐스팅 후 delete[] 해줄 것
	ICommand* Clone() const;

protected:
	CmdType_t type_ = 0;
	Cmd_t cmdId_ = 0;      // 사용자 지정 커맨드 ID값
	CmdLen_t cmdLength_ = 0; // 커맨드 길이 이때 cmdLength_는 커맨드 헤더의 크기를 더한 값으로 설정하도록한다.
	// ex) Commnad<char>의 CmdLen은 1이 아니고 5임
};

// 쓸일은 없겟지만 테스트용도
template <typename T>
struct GenericCommand : ICommand
{
	GenericCommand()
	{
		type_ = CmdType::Generic;
		cmdId_ = 0;
		cmdLength_ = static_cast<CmdLen_t>(sizeof(GenericCommand<T>)); // sizeof(T)로 할 경우 alignment 문제 때문에 커맨드길이는 T의 길이까지 포함해서 전송하도록 하자.
		value_ = T();
	}

	explicit GenericCommand(const Int16U _command)
	{
		type_ = CmdType::Generic;
		cmdId_ = _command;
		cmdLength_ = static_cast<CmdLen_t>(sizeof(GenericCommand<T>));
		value_ = T();
	}

	T value_;
};

struct Staticity
{
	static constexpr bool IS_STATIC = true;
	static constexpr bool IS_DYNAMIC = false;
};

struct Dynamicity
{
	static constexpr bool IS_STATIC = false;
	static constexpr bool IS_DYNAMIC = true;
};

struct DynamicCommandBase
{
	int count_{};
};

template <int TCmdType>
struct Command
{
	static constexpr bool IS_VALID = false;
	static constexpr bool IS_DYNAMIC = false;
	static constexpr bool IS_STATIC = false;
};

template <>
struct Command<CmdType::Static> : ICommand, Staticity
{
	static constexpr bool IS_VALID = true;
};

template <>
struct Command<CmdType::Dynamic> : ICommand, Dynamicity, DynamicCommandBase
{
	static constexpr bool IS_VALID = true;
};

using StaticCommand = Command<CmdType::Static>;
using DynamicCommand = Command<CmdType::Dynamic>;

NS_DETAIL_BEGIN
	template <typename T>
	struct IsStaticCommand : jc::IntegralConstant<bool, jc::IsBaseOf_v<Staticity, T>>
	{
	};

	template <typename T>
	struct IsDynamicCommand : jc::IntegralConstant<bool, jc::IsBaseOf_v<Dynamicity, T>>
	{
	};

	template <typename TCommand>
	struct HasDynamicFiled
	{
		template <typename = jc::DefaultEnableIf_t<std::is_union_v<TCommand::_ArrayField__>>>
		static constexpr bool Check(int*)
		{
			return true;
		}

		static constexpr bool Check(...)
		{
			return false;
		}

		static constexpr bool VALUE = Check(nullptr);
	};
NS_DETAIL_END

	template <typename T> 
	constexpr bool IsStaticCommand_v = detail::IsStaticCommand<T>::VALUE;

	template <typename T>
	constexpr bool IsDynamicCommand_v = detail::IsDynamicCommand<T>::VALUE;

	template <typename TCommand>
	constexpr bool HasDynamicField_v = detail::HasDynamicFiled<TCommand>::VALUE;

NS_JNET_END

#pragma pack(pop) // #pragma pack(push, CMD_ALIGNMENT)


/*=====================================================================================
								 커맨드 생성 규칙
					  아래 규칙에 맞게 커맨드를 생성토록 한다.
=====================================================================================*/


// 올바른 커맨드인지 체크하는 기능
// 템플릿 파라미터로 전달한 모든 타입은 ICommand를 상속받아야한다.
#define CMD_CHECK_BASE_OF_COMMAND(command)                 static_assert(jc::IsBaseOf_v<jnet::ICommand, command>, "... " #command " must be derived type of ICommand");
// 스타릭 커맨드는 무조건 통과, 다이나믹 커맨드일 경우 다이나믹 필드가 있는지 체크
#define DYNAMIC_CMD_CHECK_ARRAY_FIELD(command)   static_assert(jc::Or_v<jnet::IsStaticCommand_v<command>, jc::And_v<jnet::IsDynamicCommand_v<command>, jnet::HasDynamicField_v<command>>>, "... " #command " has no zero size array field");


// 스타릭 커맨드, 다이나믹 커맨드 공통 함수 정의
// count = 0으로 둔 이유는 static cmmand와 dynamic command간 상호호환을 위해서이다.
#define CMD_FUNC_DEF_NAME(struct)                          static constexpr const char* _Name() { return #struct; }
#define CMD_FUNC_DEF_COMMAND(cmd)                          static constexpr int _Command() { return cmd; }
#define CMD_FUNC_DEF_SIZE(...)                             JC_CONCAT_ARGS(CMD_FUNC_DEF_SIZE_, JC_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)
#define CMD_FUNC_DEF_SIZE_1(struct)                        static constexpr int _Size(int count = 0) { (void)count; return sizeof(struct); } // static command 전용
#define CMD_FUNC_DEF_SIZE_2(struct, countable_elem_type)   static constexpr int _Size(int count) { return sizeof(struct) + static_cast<int>(sizeof(countable_elem_type)) * count; } // dynamic command 전용
#define CMD_FUNC_DEF_CONSTRUCT(...)                        JC_CONCAT_ARGS(CMD_FUNC_DEF_CONSTRUCT_, JC_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)
#define CMD_FUNC_DEF_CONSTRUCT_1(struct)                   static void _Construct(void* _p, int count = 0) { (void)count; return jc::Memory::PlacementNew(*static_cast<struct*>(_p)); }
#define CMD_FUNC_DEF_CONSTRUCT_2(struct, countable_elem_type)                                                                                                     \
	static void _Construct(void* _p, int count)                                                                                                                   \
	{                                                                                                                                                            \
		jc::Memory::PlacementNew(*static_cast<struct*>(_p), count);                                                                                            \
		if constexpr (ConstructCountableElement)                                                                                                                  \
		{                                                                                                                                                        \
			for (int index = 0; index < count; ++index)                                                                                                            \
			{                                                                                                                                                    \
				void* pCountableElementPos = static_cast<char*>(_p) + sizeof(struct) + static_cast<int>(sizeof(countable_elem_type)) * index;                      \
				jc::Memory::PlacementNew(*static_cast<TCountableElement*>(pCountableElementPos));                                                                \
			}                                                                                                                                                    \
		}                                                                                                                                                        \
	}


// 다이나믹 커맨드 공통 기능 정의
#define DYNAMIC_CMD_USING_COUNTABLE_ELEMENT(element_type)                 using TCountableElement = element_type;
#define DYNAMIC_CMD_DECL_COUNTABLE_ELEMENT(contruct_countable_elem)       static constexpr bool ConstructCountableElement = contruct_countable_elem;
#define DYNAMIC_CMD_ADD_ARRAY_FIELD(elem_name)                  \
	union _ArrayField__                                                                                                                                \
	{                                                                                                                                                      \
		_ArrayField__() {}                                                                                                                              \
		~_ArrayField__() {}                                                                                                                             \
		TCountableElement elem_name;                                                                                                                        \
	} object_[1];                                                                                                                                          \
                                                                                                                                                        \
	TCountableElement* elem_name()                                                                                                                        \
	{                                                                                                                                                      \
		if (count_ <= 0)                                                                                                                                    \
		{                                                                                                                                                  \
			return nullptr;                                                                                                                                  \
		}                                                                                                                                                  \
		return &object_[0].elem_name;                                                                                                                      \
	}                                                                                                                                                      \
                                                                                                                                                        \
	TCountableElement& elem_name##At(int _index)                                                                                                          \
	{                                                                                                                                                      \
		if (_index < 0 || _index >= count_)                                                                                                                \
		{                                                                                                                                                  \
			jc_assert_msg(false, "%s 커맨드의 다이나믹 필드 갯수가 %d인데 %d인덱스에 접근을 시도했습니다.", _Name(), count_, _index);                       \
			throw jc::OutOfRangeException("");                                                                                                          \
		}                                                                                                                                                  \
		return object_[_index].elem_name;                                                                                                                  \
	}

#define STATIC_CMD_BEGIN(__struct__, __cmd_id__)                                                                                                            \
	struct __struct__ : jnet::StaticCommand                                                                                                           \
	{                                                                                                                                                      \
		__struct__(int count = 0)                                                                                                                           \
		{                                                                                                                                                  \
			(void)count;                                                                                                                                    \
			type_ = jnet::CmdType::Static;                                                                                                             \
			cmdId_ = __cmd_id__;                                                                                                                             \
			cmdLength_ = static_cast<CmdLen_t>(sizeof(__struct__));                                                                           \
		}                                                                                                                                                  \
                                                                                                                                                        \
		CMD_FUNC_DEF_NAME(__struct__)                                                                                                                     \
		CMD_FUNC_DEF_COMMAND(__cmd_id__)                                                                                                                     \
		CMD_FUNC_DEF_SIZE(__struct__)                                                                                                                     \
		CMD_FUNC_DEF_CONSTRUCT(__struct__)

#define STATIC_CMD_END };                                                                                                                                 


// @https://stackoverflow.com/questions/35196871/what-is-the-optimal-order-of-members-in-a-class
#define DYNAMIC_CMD_BEGIN(...)                          JC_CONCAT_ARGS(DYNAMIC_CMD_BEGIN_IMPL_, JC_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)
#define DYNAMIC_CMD_BEGIN_IMPL_3(__struct__, __cmd_id__, __countable_elem_type__) DYNAMIC_CMD_BEGIN_IMPL_4(__struct__, __cmd_id__, __countable_elem_type__, true)
#define DYNAMIC_CMD_BEGIN_IMPL_4(__struct__, __cmd_id__, __countable_elem_type__, __construct_countable_elem__)                                              \
	struct __struct__ : jnet::DynamicCommand                                                                                                          \
	{                                                                                                                                                      \
		DYNAMIC_CMD_USING_COUNTABLE_ELEMENT(__countable_elem_type__)                                                                                       \
		DYNAMIC_CMD_DECL_COUNTABLE_ELEMENT(__construct_countable_elem__)                                                                                  \
                                                                                                                                                        \
		__struct__(int _count)                                                                                                                              \
		{                                                                                                                                                  \
			type_ = jnet::CmdType::Dynamic;                                                                                                            \
			cmdId_ = __cmd_id__;                                                                                                                             \
			cmdLength_ = static_cast<CmdLen_t>(_Size(_count));                                                                                \
			count_ = _count;                                                                                                                                \
		}                                                                                                                                                  \
                                                                                                                                                        \
		CMD_FUNC_DEF_NAME(__struct__)                                                                                                                     \
		CMD_FUNC_DEF_COMMAND(__cmd_id__)                                                                                                                     \
		CMD_FUNC_DEF_SIZE(__struct__, __countable_elem_type__)                                                                                           \
		CMD_FUNC_DEF_CONSTRUCT(__struct__, __countable_elem_type__)

#define DYNAMIC_CMD_END	};
