/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:00:00 AM
 * =====================
 * 07. 정점/인덱스 버퍼 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/Tutorial/07_VertexIndexBuffer/07_VertexIndexBuffer_Function.h"

//////////////////////////////////////////////////////////////////////////////////////////

// 정점을 그대로 통과시키는 가장 단순한 HLSL 셰이더 소스를 반환한다. (05번과 동일 구조)
const char* PassThroughShaderSource()
{
	return R"(
struct VSInput
{
	float3 position : POSITION;
	float4 color    : COLOR0;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
};

// 정점 셰이더: NDC 좌표를 그대로 통과시킨다.
VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	output.position = float4(_input.position, 1.0f);
	output.color = _input.color;
	return output;
}

// 픽셀 셰이더: 보간된 색상을 그대로 출력한다.
float4 PSMain(VSOutput _input) : SV_TARGET
{
	return _input.color;
}
)";
}

//////////////////////////////////////////////////////////////////////////////////////////

// 인덱스 버퍼가 왜 필요한지 메모리 절약 계산을 콘솔에 출력한다. (학습용)
void PrintIndexBufferBenefit()
{
	jc::Console::WriteLine("\n[인덱스 버퍼가 필요한 이유]");
	jc::Console::WriteLine(" 사각형 = 삼각형 2개 = 정점 6개가 필요하지만, 실제 꼭짓점은 4개뿐이다.");
	jc::Console::Write(" 인덱스 없이: 정점 6개 x %d바이트 = %d바이트\n", (_s32)sizeof(sgf::VertexPC), (_s32)(6 * sizeof(sgf::VertexPC)));
	jc::Console::Write(" 인덱스 있음: 정점 4개 x %d바이트 + 인덱스 6개 x 4바이트 = %d바이트\n",
		(_s32)sizeof(sgf::VertexPC), (_s32)(4 * sizeof(sgf::VertexPC) + 6 * 4));
	jc::Console::WriteLine(" 사각형 하나는 차이가 작지만, 정점 수천 개짜리 모델은 절약이 어마어마해진다!");
	jc::Console::WriteLine(" (게다가 정점 셰이더 계산 결과도 재활용되므로 속도도 빨라진다)\n");
}
