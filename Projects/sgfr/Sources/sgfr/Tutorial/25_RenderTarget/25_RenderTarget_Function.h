/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:20:00 PM
 * =====================
 * 25. 렌더 타깃 - 보조 함수/셰이더 소스
 */

#pragma once

// 렌더 타깃의 개념과 활용처를 콘솔에 출력한다. (학습용)
void PrintRenderTargetExplanation();

// NDC 좌표기준 텍스처 출력용 사각형 정점 4개를 채운다. (UV 0~1, 흰색)
// @param _center: 사각형 중심 (NDC)
// @param _halfSize: 가로/세로 반지름 (NDC 단위, 화면 비율 보정은 호출자 몷)
void FillScreenQuad(sgf::VertexPTC* _pOutVertices4, const jc::vec2& _center, const jc::vec2& _halfSize);
