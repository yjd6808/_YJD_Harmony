/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:10:00 AM
 * =====================
 * 10. 월드 변환 - 보조 함수/셰이더 소스
 */

#pragma once

// 상수 버퍼로 변환 행렬을 받는 HLSL 셰이더 소스를 반환한다.
const char* TransformShaderSource();

// 행렬 결합과 계층 구조의 원리를 콘솔에 출력한다. (학습용)
void PrintHierarchyExplanation();
