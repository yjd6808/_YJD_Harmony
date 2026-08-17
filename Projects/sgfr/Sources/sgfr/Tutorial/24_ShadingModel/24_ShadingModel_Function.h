/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:10:00 PM
 * =====================
 * 24. 셰이딩 모델 - 보조 함수/셰이더 소스
 */

#pragma once

// 램버트/퐁/블린-퐁을 모두 지원하는 HLSL 셰이더 소스를 반환한다.
// gMode: 0=램버트(확산만), 1=퐁(반사 벡터), 2=블린-퐁(하프 벡터)
const char* ShadingShaderSource();

// 세 셰이딩 모델의 수식과 차이를 콘솔에 출력한다. (학습용)
void PrintShadingExplanation();

// UV 구(Sphere) 지오메트리를 생성해 정점/인덱스 배열을 채운다.
// 위도(rings) x 경도(segments)로 지구본처럼 자르며, UV 이음자리용으로 경도 마지막 열을 복제한다.
// @param _outVertices: 출력 정점 배열 (위치/법선/UV)
// @param _outIndices: 출력 인덱스 배열 (삼각형 목록)
// @param _radius: 구 반지름
// @param _rings: 세로 분할 수 (위도 방향)
// @param _segments: 가로 분할 수 (경도 방향)
void GenerateSphere(jc::Vector<sgf::VertexPNT>& _outVertices, jc::Vector<_u32>& _outIndices,
	_f32 _radius, _s32 _rings, _s32 _segments);
