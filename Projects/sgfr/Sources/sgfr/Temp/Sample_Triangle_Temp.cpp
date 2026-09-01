/*
 * Sample_Triangle_Temp — §7-1 설계서 샘플 컴파일 검증용 임시 파일
 * - 설계서 sgf_최종설계서_GraphicDevice_GraphicContext.md §7-1 코드 그대로
 * - SgfHeader.h → 실제 sgf 헤더 경로(sgf/SgfHeader.h)로 수정
 * - 빌드 검증 후 삭제 예정 (sgfr 프로젝트에 임시 포함)
 */

#include "sgf/SgfHeader.h"
#include "sgf/Graphics/Vertex.h"

using namespace sgf;
using namespace jc;

// §7-1 정점/인덱스/HLSL 셰이더는 튜토리얼 05와 동일한 최소 Triangle
static const char* kHlsl = R"(
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
VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	output.position = float4(_input.position, 1.0f);
	output.color = _input.color;
	return output;
}
float4 PSMain(VSOutput _input) : SV_TARGET
{
	return _input.color;
}
)";

// 설계서 §7 샘플 핵심 — 모든 호출이 헤더에 존재하는지 + 타입 정합성 검증
void Sample_Triangle_Temp_Run()
{
	Window window;
	window.Create("Sample Triangle", 800, 600);

	GraphicDevice& gd = GraphicDevice::Get();
	gd.Initialize();
	gd.CreateSwapChain(window.Handle(), 800, 600, PixelFormat::pfRgba8);
	GraphicContext& gc = gd.Context();

	const VertexPC vertices[] = {
		{ vec3(0.0f,  +0.5f, 0.0f), color(0xFF, 0x00, 0x00) },
		{ vec3(+0.5f, -0.5f, 0.0f), color(0x00, 0xFF, 0x00) },
		{ vec3(-0.5f, -0.5f, 0.0f), color(0x00, 0x00, 0xFF) },
	};
	const _u32 indices[] = { 0, 1, 2 };

	_u64 vs = gc.CreateVertexShader(kHlsl);
	_u64 ps = gc.CreatePixelShader(kHlsl);
	_u64 vb = gd.CreateVertexBuffer<VertexPC>(vertices, 3);
	_u64 ib = gd.CreateIndexBuffer(indices, 3);

	gc.SetVertexShader(vs);
	gc.SetPixelShader(ps);
	gc.SetVertexBuffer(vb);
	gc.SetIndexBuffer(ib);
	gc.SetTopology(PrimitiveTopology::ptTriangleList);
	gc.SetRasterizer(CullMode::cmNone, FillMode::fmSolid);
	gc.SetDepth(DepthMode::dmDisabled);
	gc.SetBlend(BlendMode::bmNone);

	gd.BeginFrame(color(0x14, 0x14, 0x1F));
	gc.DrawIndexed(3, 0, 0);
	gd.Present(true);
	gd.Finalize();

	window.Destroy();
}