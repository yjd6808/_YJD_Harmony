/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:40:00 AM
 * =====================
 * 05. 렌더링 파이프라인 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/Tutorial/05_RenderingPipeline/05_RenderingPipeline_Function.h"

//////////////////////////////////////////////////////////////////////////////////////////

// 이번 튜토리얼에서 사용할 HLSL 셰이더 소스를 반환한다.
//
// [셰이더란?]
//  GPU에서 실행되는 작은 프로그램. C언어와 비슷한 HLSL로 작성한다.
//  - 정점 셰이더(VS): 정점 하나마다 한 번씩 실행. 위치를 변환한다.
//  - 픽셀 셰이더(PS): 픽셀 하나마다 한 번씩 실행. 최종 색상을 결정한다.
//
// [이번 셰이더의 특징]
//  정점을 이미 NDC 좌표로 넣어주므로 VS는 아무 변환 없이 그대로 통과시킨다.
//  (변환 행렬은 10번 월드 변환 튜토리얼에서 등장!)
const char* TriangleShaderSource()
{
	return R"(
// === 정점 셰이더 입력 (C++의 VertexPC와 메모리 배치가 일치해야 함!) ===
struct VSInput
{
	float3 position : POSITION;   // VertexPC::position_
	float4 color    : COLOR0;     // VertexPC::color_
};

// === 정점 셰이더 출력 = 픽셀 셰이더 입력 ===
struct VSOutput
{
	// SV_POSITION: "이 값이 클립 공간 좌표다"라고 GPU에 알려주는 특별한 꾸미기(Semantic)
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
};

// === 정점 셰이더: 정점 3개에 대해 각각 한 번씩, 총 3번 실행된다 ===
VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	// w = 1: 위치를 나타내는 동차 좌표. (이미 NDC라 변환 없이 통과)
	output.position = float4(_input.position, 1.0f);
	output.color = _input.color;
	return output;
}

// === 픽셀 셰이더: 삼각형이 덮는 모든 픽셀에 대해 한 번씩 실행된다 ===
// 입력 color는 래스터라이저가 세 정점 색을 거리에 따라 자동으로 섞어준 값이다.
// (그래서 빨/초/파 세 정점 사이가 무지개처럼 보간되어 보인다)
float4 PSMain(VSOutput _input) : SV_TARGET
{
	return _input.color;
}
)";
}

//////////////////////////////////////////////////////////////////////////////////////////

// NDC 좌표계 설명을 콘솔에 출력한다. (학습용)
void PrintNdcExplanation()
{
	jc::Console::WriteLine("\n[NDC (Normalized Device Coordinates) 좌표계]");
	jc::Console::WriteLine(" - 화면 크기와 상관없이 항상 가로/세로 -1 ~ +1인 좌표계");
	jc::Console::WriteLine(" - 중앙이 (0, 0), 오른쪽위가 (+1, +1), 왼쪽아래가 (-1, -1)");
	jc::Console::WriteLine(" - GPU는 최종적으로 이 좌표를 뷰포트를 통해 픽셀 좌표로 바꾼다");
	jc::Console::WriteLine(" - 지금은 정점을 NDC에 직접 적었지만, 나중엔 행렬로 변환하게 된다\n");
}
