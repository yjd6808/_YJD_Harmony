/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 1:50:00 PM
 * =====================
 * 16. 블렌드 스테이트 - 보조 함수/셰이더 소스
 */

#pragma once

// 부드러운 원 텍스처를 그리는 HLSL 셰이더 소스를 반환한다.
// 곱셈 블렌드에서도 원 밖(알파 0)이 영향을 주지 않도록
// 알파로 흰색과 섞어서 출력하는 트릭이 들어있다.
const char* BlendQuadShaderSource();

// 블렌딩 공식과 네 가지 모드의 원리를 콘솔에 출력한다. (학습용)
void PrintBlendExplanation();

// 중심에서 가장자리로 갈수록 알파가 부드럽게 0으로 줄어드는
// 흰색 원 텍스처의 RGBA 픽셀 배열을 채운다. (색은 정점 채색으로 입힌다)
// @param _pOutPixels : 크기가 _size * _size * 4 이상인 출력 버퍼
void FillSoftCirclePixels(_u8* _pOutPixels, int _size);

// NDC 좌표기준 사각형 정점 4개를 채운다. (UV 0~1, 지정 색 채색)
// @param _center   : 사각형 중심 (NDC, -1~+1)
// @param _halfSize : 사각형 반지름 (NDC 단위)
void FillQuadVertices(sgf::VertexPTC* _pOutVertices4, const jc::vec2& _center, _f32 _halfSize, const jc::color& _color);
