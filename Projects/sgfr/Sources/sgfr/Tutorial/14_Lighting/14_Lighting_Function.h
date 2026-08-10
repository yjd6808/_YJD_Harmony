/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:40:00 AM
 * =====================
 * 14. 라이팅 - 보조 함수/셰이더 소스
 */

#pragma once

// 람버트 확산광 계산이 들어간 HLSL 셰이더 소스를 반환한다.
const char* LambertShaderSource();

// 람버트 법칙의 원리를 콘솔에 출력한다. (학습용)
void PrintLambertExplanation();

// 법선이 포함된 정육면체 정점 24개와 인덱스 36개를 채워준다.
// 꼭짓점은 8개지만 면마다 법선이 다르므로 면별로 정점을 분리해야 한다. (4개 x 6면 = 24개)
void FillCubeWithNormals(sgf::VertexPNT* _pOutVertices24, _u32* _pOutIndices36);
