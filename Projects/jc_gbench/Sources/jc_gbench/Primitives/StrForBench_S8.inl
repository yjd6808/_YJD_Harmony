/*
 * StrForBench_S8.inl: S8 = 실제 PooledString (S4→S8 정책 이관 검증용).
 * StrForBench 인터페이스(String 명명)를 BasicString이 직접 제공하므로 어댑터 클래스 불필요. 별칭만 둔다.
 * 헤더 가드 없음. StrForBench.h에서 네임스페이스 s8로 include한다.
 */

#include "jc/Primitives/StringImpl_SSOCtxTlsPool.h"

namespace jc_gbench { namespace STRFORBENCH_NS {

using StrForBench = jc::PooledString;

}} // namespace jc_gbench::STRFORBENCH_NS
