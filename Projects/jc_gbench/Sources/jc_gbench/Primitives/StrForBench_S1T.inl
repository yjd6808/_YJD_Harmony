/*
 * StrForBench_S1T.inl: S1T = 실제 BasicString<char, StringImpl_SSO<char>> (S1 이관 검증용).
 * StrForBench 인터페이스(String 명명)를 BasicString이 직접 제공하므로 어댑터 클래스 불필요. 별칭만 둔다.
 * 헤더 가드 없음. StrForBench.h에서 네임스페이스 s1t로 include한다.
 * BasicString의 Empty/Null은 inline static이라 StrForBench.cpp 정의가 필요 없다.
 */

#include "jc/Primitives/BasicString.h"
#include "jc/Primitives/StringImpl_SSO.h"

namespace jc_gbench { namespace STRFORBENCH_NS {

using StrForBench = jc::BasicString<char, jc::StringImpl_SSO<char>>;

}} // namespace jc_gbench::STRFORBENCH_NS
