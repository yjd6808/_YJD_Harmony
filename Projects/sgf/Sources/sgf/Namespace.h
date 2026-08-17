/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:30:00 PM
 * =====================
 * sgf(SteinsGate Framework) 네임스페이스 정의
 *
 * jc 프로젝트의 Namespace.h와 동일한 방식으로
 * 네임스페이스 시작/종료 매크로를 정의한다.
 *
 * [왜 매크로로 네임스페이스를 감싸는가?]
 * 1. 네임스페이스 이름이 바뀌어도 매크로만 수정하면 전체 프로젝트에 반영된다.
 * 2. 코드 스타일이 통일되어 어떤 파일을 열어도 구조가 한눈에 들어온다.
 * 3. jc/sg/sgcl 프로젝트와 동일한 규칙이므로 서로 이질감이 없다.
 */

#pragma once

#define NS_SGF_BEGIN		namespace sgf {
#define NS_SGF_END			}

#define USING_NS_SGF		using namespace sgf
