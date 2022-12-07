/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

namespace JCore {

  /* =============================================================== */
 // 참조자 제거해주는 템플릿
  template <typename T>
  struct RemoveReference
  {
    using Type = T;
  };

  template <typename T>
  struct RemoveReference<T&>
  {
    using Type = T;
  };

  template <typename T>
  struct RemoveReference<T&&>
  {
    using Type = T;
  };

  template <typename T>
  using RemoveReference_t = typename RemoveReference<T>::Type;

  /* =============================================================== */
  // 무브 시멘틱과 완벽한 전달
  template <typename T>
  inline constexpr RemoveReference_t<T>&& Move(T&& arg) {
    return static_cast<RemoveReference_t<T>&&>(arg);
  }

  template <typename T>
  inline constexpr T&& Forward(RemoveReference_t<T>& arg) {
    return static_cast<T&&>(arg);
  }


}

 