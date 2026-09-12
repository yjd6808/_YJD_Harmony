/*
	Comparator<String> 본문 정의. Comparator.h 끝에서 include된다 (별칭 이후).
	직접 include하지 않는다.
*/

#pragma once

#include "jc/Primitives/String.h"
#include "jc/Primitives/StaticString.h"

NS_JC_BEGIN

inline int Comparator<BasicString<_char, StringImpl_SSO<_char>>>::operator()(
	const BasicString<_char, StringImpl_SSO<_char>>& _lhs,
	const BasicString<_char, StringImpl_SSO<_char>>& _rhs)
{
	return _lhs.Compare(_rhs);
}

template <_u32 Size>
inline int Comparator<BasicString<_char, StringImpl_SSO<_char>>>::operator()(
	const BasicString<_char, StringImpl_SSO<_char>>& _lhs,
	const StaticString<Size, _char>& _rhs) const
{
	return _lhs.Compare(_rhs.Source);
}

NS_END
