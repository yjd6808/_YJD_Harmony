/*
 * 작성자: 윤정도
 * 생성일: 12/18/2022 7:16:38 PM
 * =====================================================================
 * [! WARNING !]이 파일은 어디서도 쓰이지 않는 헤더파일이다.
 * 이번에 구현한 ThreadLocal 기능을 처음에 구현했을 때 작성한 코드이다.
 * 내가 원하는 ThreadLocal 기능을 구현해냈지만
 *
 * 1. 불안전한 성능
 * 2. 기분나쁜 2중 해쉬맵 구조
 * 3. 구조적 결함
 * 때문에 폐기한 코드이다.
 * 다음부턴 이렇게 만들지 않기 위해 박제한다.
 * =====================================================================
 * [당시 내가한 생각]
 * 각 쓰레드별로 개별적으로 존재하는 객체를 저장하는 용도..
 * 템플릿이 아닌 글로벌 변수인 ThreadLocalManager와
 * 템플릿 타입인 ThreadLocal간의 호환문제 때문에 T* 타입을 void*로 뭉게서 사용한다.
 * 구조적으로 문제가 있지만 우선 구현에 목적을 둔다.
 *
 */


#pragma once

#if 0
#include <JCore/Container/Vector.h>

namespace JCore {

	struct ThreadLocalCopy
	{
		ThreadLocalCopy(void* original): OriginalOwner(original) {}
		void* OriginalOwner;
		Vector<void*> Copies;
	};

} // namespace JCore
#endif