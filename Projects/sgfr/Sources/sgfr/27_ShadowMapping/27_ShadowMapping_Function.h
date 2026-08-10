/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:40:00 PM
 * =====================
 * 27. 그림자 매핑 - 보조 함수/셰이더 소스
 */

#pragma once

// [패스 1] 빛 시점에서 깊이만 기록하는 HLSL 셰이더 소스를 반환한다.
const char* ShadowDepthShaderSource();

// [패스 2] 그림자 맵을 비교해 그늘진 곳을 어둡게 그리는 HLSL 셰이더 소스를 반환한다.
const char* ShadowSceneShaderSource();

// 그림자 매핑의 2패스 원리와 그림자 여드름(acne)/바이어스를 콘솔에 출력한다. (학습용)
void PrintShadowExplanation();

// 면별 법선을 가진 정육면체 정점 24개와 인덱스 36개를 채운다. (14번과 동일 패턴)
void FillShadowCube(sgf::VertexPNT* _pOutVertices24, _u32* _pOutIndices36);

// 위를 바라보는 바닥 평면(사각형) 정점 4개와 인덱스 6개를 채운다.
// @param _halfSize : 평면 반 변 길이 (월드 단위)
void FillGroundPlane(sgf::VertexPNT* _pOutVertices4, _u32* _pOutIndices6, _f32 _halfSize);
