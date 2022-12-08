/*
 * 직접 구현가능한 기능들은 구현하도록 하자.
 */

#pragma once


#include <JCore/TypeTraits/Checker.h>
#include <type_traits>

namespace JCore {

    // 외부에 감추고자하는 구현
    namespace Detail {   

        // 템플릿 파라미터 팩(BaseArgs)으로 전달한 타입들이 모두 Derived의 부모인지 여부
        /*
         테스트 코드(아래 8줄 그대로 복붙)
         struct A{}			;
         struct B{}			;
         struct C{}			;
         struct D:A,B,C{}	;
         struct E{}			;
         IsBaseOf_1Derived_MultipleBase_v<D, A, B>;		  // true	A, B 모두 D의 부모이므로
         IsBaseOf_1Derived_MultipleBase_v<D, A, B, C>;	  // true	A, B, C 모두 D의 부모이므로
         IsBaseOf_1Derived_MultipleBase_v<D, A, B, C, E>; // false   A, B, C 모두 D의 부모이지만 E는 아님
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

        template<bool Test, typename T = void>
        struct EnableIf {};

        template<typename T>
        struct EnableIf<true, T> { using Type = T; };

        // Test가 참이면 T1 False면 T2
        template <bool Test, typename T1, typename T2>
        struct Conditional { using Type = T1; };
        template <typename T1, typename T2>
        struct Conditional<false, T1, T2> { using Type = T2; };

        template <bool Test, bool... RestTests>
        struct Or { static constexpr bool Value = Test || Or<RestTests...>::Value; };
        template <>
        struct Or<true> { static const bool Value = true; };
        template <>
        struct Or<false> { static const bool Value = false; };

        template <bool Test, bool... RestTests>
        struct And { static constexpr bool Value = Test && Or<RestTests...>::Value; };
        template <>
        struct And<true> { static const bool Value = true; };
        template <>
        struct And<false> { static const bool Value = false; };
    }


    template <typename Lhs, typename Rhs>
    constexpr bool IsSameType_v = std::is_same_v<Lhs, Rhs>;

    // From에서 To로 암묵적 변환이 가능한지
    // 즉 To ? = From이 가능한지
    // @참고 : https://docs.microsoft.com/ko-kr/cpp/standard-library/is-convertible-class?view=msvc-170
    template <typename From, typename To>
    constexpr bool IsConvertible_v = std::is_convertible_v<From, To>;


    template <typename Base, typename Derived>
    constexpr bool IsBaseOf_v = std::is_base_of_v<Base, Derived>;


    template <typename Base, typename... DerivedArgs>
    constexpr bool IsMultipleDerived 
	    = Detail::IsMultipleDerived<Base, DerivedArgs...>::Value();

    template <typename Derived, typename... BaseArgs>
    constexpr bool IsMultipleBase_v = Detail::IsMultipleBase<Derived, BaseArgs...>::Value();

    template <typename T>
    constexpr bool IsFundamentalType_v = std::is_fundamental_v<T>;


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

    template <typename From, typename To>
    constexpr bool IsCopyAssignable_v = std::is_copy_assignable_v<From>;

    template <typename From, typename To>
    constexpr bool IsMoveAssignable_v = std::is_move_assignable_v<From>;


    template<bool Test, typename T = void>
    using EnableIf_t = typename Detail::EnableIf<Test, T>::Type;

    template<bool Test, typename T1, typename T2>
    using Conditional_t = typename Detail::Conditional<Test, T1, T2>::Type;

    template<bool... Tests>
    constexpr bool Or_v = Detail::Or<Tests...>::Value;

    template<bool... Tests>
    constexpr bool And_v = Detail::And<Tests...>::Value;
} // namespace JCore