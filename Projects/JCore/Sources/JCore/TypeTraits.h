/*
 * 직접 구현가능한 기능들은 최대한 구현하도록 하자.
 * 만들어봐야 익숙해지니까...!
 */

#pragma once


#include <JCore/TypeTraits/Checker.h>
#include <type_traits>
#include <iostream>

NS_JC_BEGIN

    
    // 외부에 감추고자하는 구현
    NS_DETAIL_BEGIN   

    
    struct __Start__ {};    // 템플릿 파라미터의 시작을 표시하는 용도로 쓰임
    struct __End__ {};      // 템플릿 파라미터의 끝을 표시하는 용도로 쓰임

    template <typename T>
    struct IsEnd : FalseType {};
    template<>
    struct IsEnd<__End__> : TrueType {};

    template <typename T>
    struct EndAssert
    {
        static_assert(!IsEnd<T>::Value, "... it's out of range! cannot find valid type");
        using Type = T;
    };

    template <typename T>
    struct IsStart : FalseType {};

    template<>
    struct IsStart<__Start__> : TrueType {};

    // 템플릿 파라미터 팩(BaseArgs)으로 전달한 타입들이 모두 Derived의 부모인지 여부
    /*
     테스트 코드(아래 8줄 그대로 복붙)
     struct A{}			;
     struct B{}			;
     struct C{}			;
     struct D:A,B,C{}	;
     struct E{}			;
     IsMultipleBase<D, A, B>;		  // true	A, B 모두 D의 부모이므로
     IsMultipleBase<D, A, B, C>;	  // true	A, B, C 모두 D의 부모이므로
     IsMultipleBase<D, A, B, C, E>; // false   A, B, C 모두 D의 부모이지만 E는 아님
     */
    // Base 타입들이 모두 Derived를 자식으로 두고 있는지 검사
    template <typename Derived, typename Base, typename... BaseArgs>
    struct IsMultipleBase {
        static constexpr bool Value() {
            if constexpr (sizeof...(BaseArgs) == 0) {
                return std::is_base_of_v<Base, Derived>;
            } else {
                return std::is_base_of_v<Base, Derived> && IsMultipleBase<Derived, BaseArgs...>::Value();
            }
        }
    };

    // 템플릿 파리미터 팩(DerivedArgs)으로 전달한 타입들이 모두 Base의 자식인지 여부
    /*
     테스트 코드(아래 6줄 그대로 복붙)
     struct A{}   ;
     struct B:A{} ;
     struct C:A{} ;
     struct D{}   ;
     IsMultipleDrived<A, B, B, C>; // true	B, B, C 모두 자식이 맞음
     IsMultipleDrived<A, B, B, D>; // false	B, B는 자식이 맞지만 D가 아님
     */
    // Derived 타입들이 모두 Base를 부모로 두고 있는지 검사
    template <typename Base, typename Derived, typename... DerivedArgs>
    struct IsMultipleDerived {
        static constexpr bool Value() {
            if constexpr (sizeof...(DerivedArgs) == 0) {
                return std::is_base_of_v<Base, Derived>;
            } else {
                return std::is_base_of_v<Base, Derived> && IsMultipleDerived<Base, DerivedArgs...>::Value();
            }
        }
    };

    template <typename T, typename... Rest>
    struct IsPrimitiveTypes {
        static constexpr bool Value() {
            if constexpr (sizeof...(Rest) == 0) {
                return std::is_fundamental_v<T>;
            } else {
                return std::is_fundamental_v<T> && IsPrimitiveTypes<Rest...>::Value();
            }
        }
    };

    template<Int64U Left, Int64U Right>
    struct Less {
        static constexpr bool Value = Left < Right;
    };

    template<Int64U Left, Int64U Right>
    struct Greater {
        static constexpr bool Value = Left > Right;
    };

    template<Int64U Left, Int64U Right>
    struct LessEqual {
        static constexpr bool Value = Left <= Right;
    };

    template<Int64U Left, Int64U Right>
    struct GreaterEqual {
        static constexpr bool Value = Left >= Right;
    };

    // SFINAE를 활용하기 위함
    template<bool Test, typename T = void>
    struct EnableIf {};

    template<typename T>
    struct EnableIf<true, T> { using Type = T; };

    // SFINAE를 활용하기 위함
    template<bool Test>
    struct DefaultEnableIf {};

    template<>
    struct DefaultEnableIf<true> { using Type = void*; };

    // Conditional<true, T1, T2> = T1
    // Conditional<false, T1, T2> = T2
    template <bool Test, typename T1, typename T2>
    struct Conditional { using Type = T1; };
    template <typename T1, typename T2>
    struct Conditional<false, T1, T2> { using Type = T2; };

    // Or<false, true, false> = true
    template <bool Test, bool... RestTests>
    struct Or { static constexpr bool Value = Test || Or<RestTests...>::Value; };
    template <>
    struct Or<true> { static const bool Value = true; };
    template <>
    struct Or<false> { static const bool Value = false; };

    // And<false, true, false> = false
    template <bool Test, bool... RestTests>
    struct And { static constexpr bool Value = Test && And<RestTests...>::Value; };
    template <>
    struct And<true> { static const bool Value = true; };
    template <>
    struct And<false> { static const bool Value = false; };


    template <typename... Args>
    struct ParameterPack
    {
        using Type = ParameterPack<Args...>;
        static constexpr int Count = sizeof...(Args);
    };

    // 파라미터팩 타입인지 여부
    template <typename T>
    struct IsParameterPack : FalseType {};
    template <typename... Args>
    struct IsParameterPack<ParameterPack<Args...>> : TrueType {};
    

    // ParameterPackCountOf<
    //      ParameterPack<int, int>,
    //      ParameterPack<double, long, float>,
    //      ParameterPack<int>,
    //      __End__> = 6
    template <typename FirstParameterPack, typename... RestParameterPacks>
    struct ParameterPackCountOf {
        static constexpr int Count = FirstParameterPack::Count + ParameterPackCountOf<RestParameterPacks>::Count;
    };

    template <>
    struct ParameterPackCountOf<__End__> { static constexpr int Count = 0; };


    template <typename Pack1, typename Pack2>
    struct ParameterPackCombineImpl
    {
        static_assert(IsParameterPack<Pack1>::Value, "... Pack1 is not ParameterPack");
        static_assert(IsParameterPack<Pack2>::Value, "... Pack2 is not ParameterPack");
    };
    template <typename... Pack1Args, template <typename...> typename Pack1
             ,typename... Pack2Args, template <typename...> typename Pack2>
    struct ParameterPackCombineImpl<Pack1<Pack1Args...>, Pack2<Pack2Args...>>
    {
        using Type = ParameterPack<Pack1Args..., Pack2Args...>;

    };

    // 전달한 파라미터팩 인자들을 합쳐주는 기능추가해보기
    template < typename FirstPack
    		 , typename... RestPacks>
    struct ParameterPackCombine {
        // 예를 들어서 ParameterPackCombine<P1, P2, P3, P4>가 전달됬다고 가정하고 흐름을 머릿속으로 그려보자.
        using Type = typename ParameterPackCombineImpl<FirstPack, typename ParameterPackCombine<RestPacks...>::Type>::Type;
    };


    template <typename FirstPack>
    struct ParameterPackCombine<FirstPack, __End__> {
        using Type = FirstPack;
    };


    // IndexOf_t<0, int, long, double> = int
    // IndexOf_t<1, int, long, double> = long
    // IndexOf_t<2, int, long, double> = double
    // IndexOf_t<3, int, long, double> = __End__
    template <Int Index, Int TargetIndex, typename First, typename... Rest>
    struct IndexOf {
        using Type = typename Conditional<Index == TargetIndex,
            First,
            typename IndexOf<Index + 1, TargetIndex, Rest...>::Type>::Type;
    };

    template <Int Index, Int TargetIndex>
    struct IndexOf<Index, TargetIndex, __End__> {
        using Type = __End__;
    };


    // 함수 반환타입/매개변수 타입 가져오기
    // 람다는 어케해야할지 몰겠다.
    // template <typename R, typename... Args>
    // R fn(Args&&... args) { return R(); }
    template <typename Fn>
    struct FuncSig
    {
        using Parameters = ParameterPack<__End__>;
        using Return = __End__;
        static constexpr bool Valid = false;
    };


    template <typename R, typename... Args>
    struct FuncSig<R(*)(Args...)>
    {
        using Parameters = Detail::ParameterPack<Args...>;
        using Return = R;
        static constexpr bool Valid = true;
    };

    template <typename R, typename... Args>
    struct FuncSig<R(Args...)>
    {
        using Parameters = Detail::ParameterPack<Args...>;
        using Return = R;
        static constexpr bool Valid = true;
    };

    template <typename R, typename... Args>
    struct FuncSig<R(&)(Args...)>
    {
        using Parameters = Detail::ParameterPack<Args...>;
        using Return = R;
        static constexpr bool Valid = true;
    };

   

    // JCore::Allocator에서 정의된 할당자들이 규칙을 준수해서 제대로 정의되었는지 검사용도
    // 함수가 없는 경우도 체크하고싶은데...
    // 이 안에서 오류가 발생하는 경우 할당자 정의가 안된 것으로 가정하도록 하자.
    template <typename T, typename TAllocator>
    struct IsValidAllocator
    {
        using Type = NakedType_t<T>;

        // Allocate 함수 리턴타입 검사
        static constexpr bool TestAllocateReturn1() { return IsPointerType<decltype(TAllocator::template Allocate<Type>())>::Value; }
        static constexpr bool TestAllocateReturn2() { return IsPointerType<decltype(TAllocator::template Allocate<Type*>())>::Value; }
        static constexpr bool TestAllocateReturn3(int dummy = 0) { return IsPointerType<decltype(TAllocator::template Allocate<Type>(dummy, dummy))>::Value; }
        static constexpr bool TestAllocateReturn4(int dummy = 0) { return IsPointerType<decltype(TAllocator::template Allocate<Type*>(dummy, dummy))>::Value; }


        // Allocate 함수 파라미터 타입 검사
        template <typename Pack>
        struct AllocateParamCheck : FalseType {};

        template <template<typename...> typename Pack, typename... Args>
        struct AllocateParamCheck<Pack<Args...>> : FalseType {};

        // Static
        template <template<typename...> typename Pack>
        struct AllocateParamCheck<Pack<void>> : TrueType {};

        // Dynamic
        template <template<typename...> typename Pack>
        struct AllocateParamCheck<Pack<int, int&>> : TrueType {};

        static constexpr bool TestAllocateParam1() {
            // Dynamic Allocate 함수는
            // 디폴트 템플릿 파라미터를 설정해놔서
            // 무조건 타입을 명시해서 전달해야하는 Static Allocate와 구분되도록 한다.
            using Signature = decltype(TAllocator::template Allocate<>); 
            using Param = typename FuncSig<Signature>::Parameters;
            return AllocateParamCheck<Param>::Value;
        }
        
        static constexpr bool IsValidAllocateReturnType = TestAllocateReturn1() && TestAllocateReturn2() && TestAllocateReturn3() && TestAllocateReturn4();
        static constexpr bool IsValidAllocateParamType = TestAllocateParam1();


        // ============================================================================

        // Deallocate 함수 리턴 검사
        static constexpr bool TestDeallocateReturn1(void* dummy = nullptr) { return IsVoidType_v<decltype(TAllocator::template Deallocate<Type>(dummy))>; }
        static constexpr bool TestDeallocateReturn2(void* dummy1 = nullptr, int dummy2 = 0) { return IsVoidType_v<decltype(TAllocator::template Deallocate(dummy1, dummy2))>; }
     
        // Deallocate 함수 파라미터 타입 검사
        template <typename Pack>
        struct DeallocateParamCheck : FalseType {};

        template <template<typename...> typename Pack, typename... Args>
        struct DeallocateParamCheck<Pack<Args...>> : FalseType {};

        // Static
        template <template<typename...> typename Pack>
        struct DeallocateParamCheck<Pack<void*>> : TrueType {};

        // Dynamic
        template <template<typename...> typename Pack>
        struct DeallocateParamCheck<Pack<void*, int>> : TrueType {};

        // Static
        static constexpr bool TestDeallocateParam1() {
            using Signature = decltype(TAllocator::template Deallocate<Type>);
            using Param = typename FuncSig<Signature>::Parameters;
            return DeallocateParamCheck<Param>::Value;
        }

        // Dynamic
        static constexpr bool TestDeallocateParam2() {
            using Signature = decltype(TAllocator::Deallocate);
            using Param = typename FuncSig<Signature>::Parameters;
            return DeallocateParamCheck<Param>::Value;
        }
      


        static constexpr bool IsValidDeallocateReturnType = TestDeallocateReturn1() && TestDeallocateReturn1();
        static constexpr bool IsValidDeallocateParamType = TestDeallocateParam1() && TestDeallocateParam2();
        static constexpr bool Valid = IsValidAllocateReturnType && IsValidAllocateParamType && IsValidDeallocateParamType && IsValidDeallocateParamType;
    };
       
    NS_DETAIL_END


template <typename Lhs, typename Rhs>
constexpr bool IsSameType_v = std::is_same_v<Lhs, Rhs>;

// From에서 To로 암묵적 변환이 가능한지
// 즉 To ? = From이 가능한지
// @참고 : https://docs.microsoft.com/ko-kr/cpp/standard-library/is-convertible-class?view=msvc-170
template <typename From, typename To>
constexpr bool IsConvertible_v = std::is_convertible_v<From, To>;

template <typename From, typename... Args>
constexpr bool IsConstructible_v = std::is_constructible_v<From, Args...>;


template <typename Base, typename Derived>
constexpr bool IsBaseOf_v = std::is_base_of_v<Base, Derived>;


template <typename Base, typename... DerivedArgs>
constexpr bool IsMultipleDerived_v 
    = Detail::IsMultipleDerived<Base, DerivedArgs...>::Value();

template <typename Derived, typename... BaseArgs>
constexpr bool IsMultipleBase_v = Detail::IsMultipleBase<Derived, BaseArgs...>::Value();

template <typename T>
constexpr bool IsFundamentalType_v = std::is_fundamental_v<T>;

template <typename T>
constexpr bool IsEnumType_v = std::is_enum_v<T>;

template <typename T, typename... Rest>
constexpr bool IsPrimitiveTypes_v = Detail::IsPrimitiveTypes<T, Rest...>::Value();


// 서로 다이나믹 캐스팅이 가능한지 알려주는 녀석
// 템플릿 인자로 전달한 녀석이 둘 모두 포인터 타입 또는 레퍼런스 타입인 경우
// 원시 타입은 원래 다이나믹 캐스팅이 안되지만 같은 타입끼리는 걍 된다고 처리
template <typename Lhs, typename Rhs>
constexpr bool IsDynamicCastable_v =(IsPointerType_v<Lhs> && IsPointerType_v<Rhs>)  ||
                                    (IsReferenceType_v<Lhs> && IsReferenceType_v<Rhs>) 
							        ?
									    IsSameType_v<Lhs, Rhs>	// 그냥 서로 동일한 타입이면 무조건 OK
									    ? 
									    true
									    :
									    IsBaseOf_v<RemovePointer_t<RemoveReference_t<Lhs>>, RemovePointer_t<RemoveReference_t<Rhs>>> ||
									    IsBaseOf_v<RemovePointer_t<RemoveReference_t<Rhs>>, RemovePointer_t<RemoveReference_t<Lhs>>>
							        :
							        false;

// From이 To의 부모 타입인지 포인터, 참조타입 인경우에 비교
template <typename From, typename To>
constexpr bool IsRPBaseOf_v = (IsPointerType_v<From> && IsPointerType_v<To>)  ||
						    (IsReferenceType_v<From> && IsReferenceType_v<To>)
							    ?
							    IsBaseOf_v<RemovePointer_t<RemoveReference_t<From>>, RemovePointer_t<RemoveReference_t<To>>>
							    :
							    false;


template <typename From, typename To>
constexpr bool IsAssignable_v = std::is_assignable_v<From, To>;

template <typename From>
constexpr bool IsCopyAssignable_v = std::is_copy_assignable_v<From>;

template <typename From>
constexpr bool IsMoveAssignable_v = std::is_move_assignable_v<From>;

template <typename From>
constexpr bool IsCopyConstructible_v = std::is_copy_constructible_v<From>;

template <typename From>
constexpr bool IsMoveConstructible_v = std::is_move_constructible_v<From>;

template <typename Fn>
constexpr bool IsFunction_v = std::is_function_v<Fn>;


template<bool Test, typename T = void>
using EnableIf_t = typename Detail::EnableIf<Test, T>::Type;

template<bool Test>
using DefaultEnableIf_t = typename Detail::DefaultEnableIf<Test>::Type;

template<bool Test, typename T1, typename T2>
using Conditional_t = typename Detail::Conditional<Test, T1, T2>::Type;

template<bool... Tests>
constexpr bool Or_v = Detail::Or<Tests...>::Value;

template<bool... Tests>
constexpr bool And_v = Detail::And<Tests...>::Value;

template <typename... Args>
constexpr bool CountOf_v = Detail::ParameterPack<Args...>::Count;

template <typename T>
constexpr bool IsEnd_v = Detail::IsEnd<T>::Value;

template <typename T>
constexpr bool IsStart_v = Detail::IsStart<T>::Value;

template<Int Index, typename... Args>
using IndexOf_t = typename Detail::EndAssert<typename Detail::IndexOf<0, Index, Args..., Detail::__End__>::Type>::Type;

template <typename Fn>
using FuncSigParam_t = typename Detail::FuncSig<Fn>::Parameters;

template <typename Fn>
using FuncSigReturn_t = typename Detail::FuncSig<Fn>::Return;


template<typename... Args>
using ParameterPack_t = typename Detail::ParameterPack<Args...>::Type;


// ParameterPackCombine_t<ParameterPack_t<int, int>, ParameterPack_t<double, double>, ParameterPack_t<long, long, long>>
//    = ParameterPack<int, int, double, double, long, long, long>
// 이렇게 합쳐줌
template<typename... ParameterPacks>
using ParameterPackCombine_t = typename Detail::ParameterPackCombine<ParameterPacks..., Detail::__End__>::Type;


template<typename T, typename Allocator>
constexpr bool IsValidAllocator_v = Detail::IsValidAllocator<T, Allocator>::Valid;


NS_JC_END