/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:40:00 AM
 * =====================
 * 04. 렌더링 파이프라인 - 보조 함수/셰이더 소스
 */

#pragma once

// 이번 튜토리얼에서 사용할 HLSL 셰이더 소스를 반환한다.
// (파일로 분리하지 않고 문자열로 내장해서 복사/실행이 간편하도록 함)
const char* TriangleShaderSource();

// NDC 좌표계 설명을 콘솔에 출력한다. (학습용)
void PrintNdcExplanation();
