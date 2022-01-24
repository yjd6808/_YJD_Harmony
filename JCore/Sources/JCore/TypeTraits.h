#pragma once

#include <JCore/String.h>

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
inline String Type() {
	String szFuncSig(__FUNCSIG__);

	const int kiLeftIdx = szFuncSig.Find("<");
	const int kiRightIdx = szFuncSig.FindReverse(">");

	return szFuncSig.GetRange(kiLeftIdx + 1, kiRightIdx - 1);
}

} // namespace JCore