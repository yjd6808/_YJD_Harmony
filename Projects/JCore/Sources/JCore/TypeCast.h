/*
 * 작성자: 윤정도
 * =====================
 *
 */

#pragma once

#include <JCore/TypeTraits/Eliminator.h>

NS_JC_BEGIN

/* =============================================================== */
// 무브 시멘틱과 완벽한 전달
template <typename T>
constexpr RemoveReference_t<T>&& Move(T&& arg) {
return static_cast<RemoveReference_t<T>&&>(arg);
}

template <typename T>
constexpr T&& Forward(RemoveReference_t<T>& arg) {
return static_cast<T&&>(arg);
}

template <typename T>
constexpr T* AddressOf(T& arg) {
  return __builtin_addressof((RemoveQulifier_t<T>&)arg);
}

template<typename T>
T* ToPtr(T& obj) { return &obj; }
template<typename T>
T* ToPtr(T* obj) { return obj; }

NS_JC_END

 