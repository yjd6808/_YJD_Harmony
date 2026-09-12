/*
	작성자 : 윤정도
	문자열 별칭 허브 (TCHAR 전환)
*/

#pragma once

#include "jc/Namespace.h"
#include "jc/Type.h"
#include "jc/Define.h"
#include "jc/Primitives/BasicString.h"

NS_JC_BEGIN

template <typename CharT>
class StringImpl_SSO;

using AString = BasicString<char,	 StringImpl_SSO<char>>;
using WString = BasicString<wchar_t, StringImpl_SSO<wchar_t>>;
using String  = BasicString<_char,	 StringImpl_SSO<_char>>;

NS_END

#include "jc/Primitives/StringImpl_SSO.h"
#include "jc/Primitives/BasicString.inl"
