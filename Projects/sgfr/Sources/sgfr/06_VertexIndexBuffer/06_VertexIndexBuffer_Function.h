/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:00:00 AM
 * =====================
 * 06. 정점/인덱스 버퍼 - 보조 함수/셰이더 소스
 */

#pragma once

// 정점을 그대로 통과시키는 가장 단순한 HLSL 셰이더 소스를 반환한다. (04번과 동일 구조)
const char* PassThroughShaderSource();

// 인덱스 버퍼가 왜 필요한지 메모리 절약 계산을 콘솔에 출력한다. (학습용)
void PrintIndexBufferBenefit();
