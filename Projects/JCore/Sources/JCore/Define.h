#if !defined(__JCORE_DEFINE_H__)

    #define __JCORE_DEFINE_H__

	// https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
	#define JCORE_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
	#define JCORE_PASS do { int JCORE_CONCAT_COUNTER(__pass__); } while(0)

	#ifdef DebugMode
		#define JCORE_INLINE_HEADER_MESSAGE(format, ...) inline auto JCORE_CONCAT_COUNTER(_) = [] { return Console::WriteLine(format, __VA_ARGS__); }()
		#define JCORE_INLINE_CLASS_MESSAGE(format, ...)	inline static auto JCORE_CONCAT_COUNTER(_) = [] { return Console::WriteLine(format, __VA_ARGS__); }()
		#define JCORE_INLINE_RETURN_MESSAGE(type, value, format, ...) []()->type { Console::WriteLine(format, __VA_ARGS__); return value; }()
	#else
		#define JCORE_INLINE_HEADER_MESSAGE(format) 
		#define JCORE_INLINE_CLASS_MESSAGE(format) 
		#define JCORE_INLINE_RETURN_MESSAGE(type, value, format, ...)	value
	#endif
	/* !! 주의해서 쓸 것 !!
	 * 헤더파일에서 사용시 멀티플 TU에서 정의되지 않도록 JCoreInlineHeaderMessage를 사용하고
	 * 클래스내에서 사용시에는 JCoreInlineClassMessage를 사용하도록 한다.
	 * 클래스내에서의 static 키워드와 글로벌 영역에서 static 키워드의 역할이 틀림!
	 * 기초적인 내용인데 자주 까먹는다.
	 *  - https://stackoverflow.com/questions/14349877/static-global-variables-in-c
	 *  - https://stackoverflow.com/questions/18841414/declaring-a-global-and-a-static-variable
	 *
	 * [예시]
	 * a.h에 inline static int test = 0; 을 선언하고
	 * 1.cpp, 2.cpp, 3.cpp, 4.cpp에서 이 헤더파일을 포함시키면
	 *
	 * 1.cpp에도 test가 1개
	 * 2.cpp에도 test가 1개
	 * 3.cpp에도 test가 1개
	 * 4.cpp에도 test가 1개
	 * 프로그램에 총 4개의 test가 존재하게 된다.
	 *
	 * a.h에 inline int test = 0;와 같이 선언하면
	 * 1.cpp ~ 4.cpp 통틀어서 1개를 만들게됨
	 */


    #define JCORE_STDCALL        __stdcall
    #define JCORE_CDECL          __cdecl
    #define JCORE_FORCEINLINE    __forceinline
    #define JCORE_INFINITE       0xffffffff

    #define JCORE_MAKE_NULL(x	)		\
    do {								\
        (x) = nullptr;					\
    } while (0)
    #define JCORE_DELETE_SAFE(x)		\
    do {								\
	    if (x) {						\
		    delete (x);					\
        (x) = nullptr;					\
	    }								\
    } while (0)

	#define JCORE_DELETE_SINGLETON_SAFE(x)	\
    do {									\
	    if (x) {							\
		    x->Free();						\
        (x) = nullptr;						\
	    }									\
    } while (0)

    #define JCORE_DELETE_ARRAY_SAFE(x)	\
    do {								\
	    if (x) {						\
		    delete[] (x);				\
        (x) = nullptr;		  			\
	    }								\
    } while (0)		

	#define JCORE_RELEASE_SAFE(x)		\
    do {								\
	    if (x) {						\
		    x->Release();				\
        (x) = nullptr;					\
	    }								\
    } while (0)

	#define JCORE_ALLOCATOR_STATIC_DEALLOCATE_SAFE(type, ptr)		\
    do {															\
	    if ((ptr)) {												\
		    TAllocator::template DeallocateStatic<type>(ptr);		\
			(ptr) = nullptr;		  								\
	    }															\
    } while (0)		

	#define JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(type, ptr)		\
    do {																		\
	    if ((ptr)) {															\
			Memory::PlacementDelete(ptr);										\
		    TAllocator::template DeallocateStatic<type>(ptr);					\
			(ptr) = nullptr;		  											\
	    }																		\
    } while (0)		

	#define JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(ptr, size)		\
	do {													\
	    if ((ptr)) {										\
		    TAllocator::DeallocateDynamic((ptr), (size));	\
			(ptr) = nullptr;		 						\
	    }													\
    } while (0)		



	#define	JCORE_PLACEMENT_DELETE_ARRAY_SAFE(arr, size)				\
	do {													\
	    if ((arr)) {										\
		    Memory::PlacementDeleteArray((arr), (size));	\
	    }													\
    } while (0)	
	
	#define JCORE_LEAK_CHECK_ASSERT		JCore::AutoMemoryLeakDetector JCORE_CONCAT_COUNTER(_) {[](Int32U leakedBytes ) { DebugAssertMsg(leakedBytes == 0, "%u 바이트 메모리 릭이 있습니다.", leakedBytes); }}
	#define JCORE_DEFAULT_CODE_PAGE		JCore::CodePage::UTF8

	// 파라미터 어노테이션
	#define JCORE_REF_IN
	#define JCORE_REF_INOUT
	#define JCORE_REF_OUT

    #define JCORE_IN
    #define JCORE_OUT
	#define JCORE_OUT_OPT
    #define JCORE_IN_OUT
    #define JCORE_IN_OPT

	// 필드 어노테이션
	#define JCORE_NOT_NULL
	#define JCORE_NULLABLE
	#define JCORE_UNUSED
	
	#define JCORE_SWAP(val1, val2, type) do { type temp = val1; val1 = val2; val2 = temp; } while (0)

	// Cocos2d-x에서 자주사용하던 바인딩
	#define JCORE_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
	#define JCORE_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
	#define JCORE_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
	#define JCORE_CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)

	// 부모 클래스 팡션 숨기는법
	// https://stackoverflow.com/questions/4908539/a-way-in-c-to-hide-a-specific-function
	#define JCORE_HIDE_BASE_CLASS_METHOD(base_class_type, method) private: using base_class_type::method;


	 // enum class 쓰긴 싫고 enum은 타입 이름이 겹쳐서 싫고 대안으로 struct안에 enum 끼여서 쓰는건 어떨까?
	 // 이번에 개발 할때 써보고 영 아니면 폐기하는걸로, 쓴지 하루지났는데 아직 갠찮은듯
	#define JCORE_SENUM_BEGIN(enum_name) struct enum_name { enum_name() = delete; enum _ {
	#define JCORE_SENUM_INHERIT_BEGIN(enum_name, base_enum) struct enum_name : base_enum { enum_name() = delete; enum _ {
	#define JCORE_SENUM_MIDDLE(enum_name) }; using enum_name##_t = enum_name::_;
	#define JCORE_SENUM_MIDDLE_END(enum_name) }; using enum_name##_t = enum_name::_;
	#define JCORE_SENUM_END(enum_name) }; }; using enum_name##_t = enum_name::_;


	#define JCORE_ENUM_CLASS_BIT_OPERATION_OVERLOADING(enum_class)			\
	enum class enum_class;													\
	constexpr enum_class operator&(enum_class lhs, enum_class rhs) {		\
		return enum_class(int(lhs) & int(rhs));								\
	}																		\
																			\
	constexpr enum_class operator|(enum_class lhs, enum_class rhs) {		\
		return enum_class(int(lhs) | int(rhs));								\
	}																		\
																			\
	constexpr enum_class operator^(enum_class lhs, enum_class rhs) {		\
		return enum_class(int(lhs) ^ int(rhs));								\
	}																		\
																			\
	constexpr enum_class operator~(enum_class lhs) {						\
		return enum_class(~int(lhs));										\
	}																		\
																			\
	inline enum_class& operator&=(enum_class& x, enum_class y) {			\
		x = x & y;															\
		return x;															\
	}																		\
																			\
	inline enum_class& operator|=(enum_class& x, enum_class y) {			\
		x = x | y;															\
		return x;															\
	}																		\
																			\
	inline enum_class& operator^=(enum_class& x, enum_class y) {			\
		x = x ^ y;															\
		return x;															\
	}


	#define JCORE_FORWARD_TEMPLATE(n)	JCORE_FORWARD_TEMPLATE_##n
	#define JCORE_FORWARD_TEMPLATE_1	template <typename>
	#define JCORE_FORWARD_TEMPLATE_2	template <typename, typename>
	#define JCORE_FORWARD_TEMPLATE_3	template <typename, typename, typename>
	#define JCORE_FORWARD_TEMPLATE_4	template <typename, typename, typename, typename>
#endif