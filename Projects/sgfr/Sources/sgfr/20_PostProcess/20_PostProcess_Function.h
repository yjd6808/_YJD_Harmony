/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:30:00 PM
 * =====================
 * 20. 포스트 프로세싱 - 보조 함수/셰이더 소스
 */

#pragma once

// 화면 전체 텍스처에 후보정 효과를 입혀 출력하는 HLSL 셰이더 소스를 반환한다.
// gMode에 따라 그레이/세피아/반전/비네트/물결 효과를 적용한다.
const char* PostProcessShaderSource();

// 포스트 프로세싱의 개념과 각 효과의 수식을 콘솔에 출력한다. (학습용)
void PrintPostProcessExplanation();

// 화면 전체를 덮는 NDC 사각형 정점 4개를 채운다. (UV 0~1, 흰색)
void FillFullscreenQuad(sgf::VertexPTC* _pOutVertices4);
