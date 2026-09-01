/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:00:00 AM
 * =====================
 * sgfr 프리컴파일 헤더 (base.props의 ForcedIncludeFiles로 모든 cpp에 자동 포함)
 *
 * [프리컴파일 헤더(PCH)란?]
 * 자주 바뀌지 않는 무거운 헤더(Windows.h, STL, jc, sgf)를 한 번만 컴파일해두고
 * 모든 cpp가 재사용하는 기법. 빌드 속도가 크게 빨라진다.
 */

#pragma once

// === 표준 라이브러리 ===
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>

// === jc + sgf 엔진 통합 (sgf PCH가 Windows/jc/D3D11 포함) ===
#include <sgf/PCH/Core.h>
#include <sgf/SgfHeader.h>