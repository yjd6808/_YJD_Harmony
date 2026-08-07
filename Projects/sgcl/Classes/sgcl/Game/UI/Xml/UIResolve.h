/*
 * 작성자: 윤정도
 * =====================
 * sgui - WPF 스타일 XML 속성 리졸브 엔진
 *
 * - 중괄호 토큰: {#Root.Width}, {@Player.Name}, {$parent.Width}, {$ancestor(2).Width}, {$prev.Width}
 * - 요소 참조: #Name (접두사 필수 — name 기반 요소 조회), 체인 {#a.b.Width}, 컬렉션 {#List[0].Width}
 * - 데이터 참조: @Key, @Map.Key, @Map.Nested.Key, @Array[0], 중첩 접근 {@Items[{@Index}]}
 * - 상대 참조: $parent / $root / $self / $prev / $next / $ancestor(n), 체인 $parent.$parent
 * - 수식: + - * / % ( ), 비교 == != < <= > >=, 논리 && || !, 문자열 +, max()/min()
 * - 문자열 리터럴: 백틱 `` `Hello` ``
 * - 해석 규칙: 중괄호 내부가 전체 표현식 ({#a.Width + 30}). 중괄호/백틱 밖의 연산 토큰은 미지원
 * - 해석 순서: 단일 ` ` → 문자열 리터럴, 단일 { } → 내부 전체 표현식 평가 → 실패 시 중괄호 치환(문자열 보간) 폴백
 */

#pragma once

#include "sgcl/Game/UI/Xml/UIValue.h"

namespace sgui
{

class UIElement;

struct UIResolveContext
{
	UIElement* pCurrent = nullptr;			// 속성을 적용받는 요소
	UIElement* pRoot = nullptr;				// 윈도우 루트 (이름 검색 기준)
	const UIDataList* pData = nullptr;		// 윈도우 데이터 (DataList)
};

// 속성 값 전체를 해석한다. 표현식이 성립하면 타입 보존 결과, 아니면 문자열 보간 결과.
UIValue ResolveAttribute(const jc::String& _raw, const UIResolveContext& _ctx);

// 중괄호 내부 표현식 평가 (실패 시 Null).
UIValue ResolveExpression(const jc::String& _expr, const UIResolveContext& _ctx);

// 요소 프로퍼티 읽기 (Width/Height/Margin.Left/Text/IsChecked/Percent/Value/...).
bool ResolveElementProperty(UIElement* _pElement, const jc::String& _property, UIValue& _out);

// 중괄호 토큰만 문자열 치환 (표현식이 아닌 텍스트 보간용).
jc::String ResolveInterpolate(const jc::String& _raw, const UIResolveContext& _ctx);

} // namespace sgui
