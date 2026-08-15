/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 1:40:00 PM
 * =====================
 * 17. 샘플러와 필터링 - 보조 함수/셰이더 소스
 */

#pragma once

// 텍스처를 입힌 사각형을 그리는 HLSL 셰이더 소스를 반환한다.
// 정점 위치를 NDC 좌표 그대로 사용하므로 행렬 없이 동작한다.
const char* TextureQuadShaderSource();

// 샘플러/필터링의 원리를 콘솔에 출력한다. (학습용)
void PrintSamplerExplanation();

// 체커보드 패턴의 RGBA 픽셀 배열을 채운다.
// 가장자리 1픽셀은 빨간색 테두리로 채워서 주소 모드(Wrap/Clamp/Mirror)의
// 차이가 한눈에 보이도록 한다.
// @param _pOutPixels : 크기가 _width * _height * 4 이상인 출력 버퍼
// @param _cellSize   : 체커 한 칸의 픽셀 크기
void FillCheckerPixels(_u8* _pOutPixels, _s32 _width, _s32 _height, _s32 _cellSize);

// UV가 0~_uvScale 범위인 사각형 정점 4개와 인덱스 6개를 채운다.
// _uvScale이 1보다 크면 UV가 0~1 범위를 벗어나므로 주소 모드 차이를 관찰할 수 있다.
void FillUvQuad(sgf::VertexPTC* _pOutVertices4, _u32* _pOutIndices6, _f32 _uvScale);
