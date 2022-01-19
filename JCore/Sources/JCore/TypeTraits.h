#pragma once

namespace JCore {


template <typename T>
struct RemoveReference
{
	using Type = typename T;
};

template <typename T>
struct RemoveReference<T&>
{
	using Type = typename T;
};

template <typename T>
struct RemoveReference<T&&>
{
	using Type = typename T;
};

template <typename T>
using RemoveReference_t = typename RemoveReference<T>::Type;

template <typename T>
inline constexpr RemoveReference_t<T>&& Move(T&& arg) {
	return static_cast<RemoveReference_t<T>&&>(arg);
}

template <typename T>
inline constexpr T&& Forward(RemoveReference_t<T>& arg) {
	return static_cast<T&&>(arg);
}

template <typename T>
inline constexpr const char* Type() {
	return (const char*)__FUNCSIG__;
}

} // namespace JCore